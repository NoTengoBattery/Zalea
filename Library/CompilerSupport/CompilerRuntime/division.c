//===-- division.c - Divide Integer Numbers ---------------------------------------------------------------*- C -*-===//
//
// Copyright (c) 2020 Oever González
//
//  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
//  the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This file tries to implement division of integer numbers. This is probably not fast at all but it's required by
/// targets that does not support integer division such as the ARM (some CPUs do support integer division but the
/// compiler will know). In theory, this code is an implementation of the large binary division.
///
/// This is probably the slowest possible implementation but anyway... it does work good.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/BitwiseMacros.h>
#include <CompilerMagic/CompilerMagic.h>
#include <limits.h>

/// If the sign bit flag is enabled, the number is positive.
#define SIGN_FLAG 0x00U
/// If the div0 bit flag is enabled, a division by zero has occurred.
#define DIV_0_FLAG 0x01U

/// \brief This special struct defines a number with flags.
///
/// All numbers in this struct are signed, but the value is unsigned and it's sign is defined by the sign bit in the
/// flags field. A special process is required to build a number from a *normal* number.
struct signedT {
    /// The flags of this number. Flags are defined by the macros in this file.
    unsigned char flags;
    /// The unsigned value of this number.
    unsigned long value;
};

/// \brief This is a struct that represents a division. It has two fields, which are signedT values.
///
/// Thus, this struct represents a signed division between two integer numbers.
struct divisionT {
    struct signedT numerator;
    struct signedT denominator;
};

/// \brief This is the result of the division operation.
///
/// This struct is designed to return the values of the quotient and remainder after the division, thus returning both
/// results in the same call.
struct resultT {
    /// Remainder is always positive or 0, by definition
    unsigned long remainder;
    /// The signedT struct is to recycle the signed division for unsigned values, just discarding the sign.
    struct signedT quotient;
};

/// \brief Perform a long division.
///
/// This function will perform a long signed division between the 2 integers provided and will return the result by
/// changing the values in the struct provided by the caller.
/// \param operands a divisionT that represents the two operands.
/// \param result a pointer to a resultT struct that this function will modify to return the results.
void longDivision(struct divisionT *operands, struct resultT *result) {
    // Unpack the structs for better readability
    unsigned long denominator = operands->denominator.value;
    unsigned char denominatorFlags = operands->denominator.flags;
    unsigned long numerator = operands->numerator.value;
    unsigned char numeratorFlags = operands->numerator.flags;
    // Handle special cases ...
    //  1. Division by zero: quotient is ULONG_MAX, remainder is zero and set the DIV0 flag of the quotient
    if (denominator == 0x00U) {
        result->quotient.value = ULONG_MAX;
        result->quotient.flags = SET_NTH_BIT(0x00U, DIV_0_FLAG);
        result->remainder = 0x00UL;
        return;
    }
    //  2. Division by one: the quotient is the number and the remainder is zero
    if (denominator == 0x01U) {
        result->quotient.value = numerator;
        result->quotient.flags = (unsigned char) XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);  // NOLINT
        result->remainder = 0x00UL;
        return;
    }
    //  3. Division by itself: the quotient is one and the remainder is zero
    if (denominator == numerator) {
        result->quotient.value = 0x01U;
        result->quotient.flags = 0x00U;
        result->remainder = 0x00UL;
        return;
    }
    //  4. Division by a larger denominator: the quotient is zero and the remainder is the numerator
    if (denominator > numerator) {
        result->quotient.value = 0x00U;
        result->quotient.flags = (unsigned char) XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);  // NOLINT
        result->remainder = numerator;
        return;
    }
    // Shift the denominator until it's bigger or equal than the numerator
    unsigned long shiftedDenominator = denominator;
    unsigned int steps = 0x00U;
    while (shiftedDenominator <= numerator) {
        shiftedDenominator = shiftedDenominator << 0x01U;
        steps += 1;
    }
    shiftedDenominator = shiftedDenominator >> 0x01U;
    // Do the long division based on the shifted denominator calculated above
    unsigned long quotient = 0x00U;
    unsigned long remainder = numerator;
    for (unsigned int i = 0x00; i < steps; ++i) {
        if (remainder >= shiftedDenominator) {
            remainder -= shiftedDenominator;
            quotient = quotient << 0x01U;
            quotient += 0x01U;
        } else {
            quotient = quotient << 0x01U;
        }
        shiftedDenominator = shiftedDenominator >> 0x01U;
    }
    // Set the struct values and return
    result->quotient.value = quotient;
    result->quotient.flags = (unsigned char) XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);  // NOLINT
    result->remainder = remainder;
}

#if defined(KERNEL_ARM) && defined(KERNEL_COMPILER_GNU)

#include <stdint.h>

/// \brief This is the EABI call for unsigned integer division with quotient.
///
/// In theory, the return quotient will go on R0 and the remainder in R1, but we can't control that (directly) from C.
/// But the ABI says that a 64 bit value will, indeed, be returned that way, so we return a 64 bit value which is the
/// composition of both the remainder and quotient. This should work as long as EABI is used (and this function won't be
/// called if you don't use EABI, anyway).
/// \param numerator (parameter introduced by the compiler)
/// \param denominator (parameter introduced by the compiler)
/// \return a composite value of the results which conforms with the EABI call.
ATTR_USED uint64_t __aeabi_uidivmod(unsigned long numerator, unsigned long denominator) {
    struct divisionT division = {
            .denominator.flags = SET_NTH_BIT(0x00U, SIGN_FLAG),
            .denominator.value = denominator,
            .numerator.flags =  SET_NTH_BIT(0x00U, SIGN_FLAG),
            .numerator.value = numerator
    };
    struct resultT result = {
            .quotient.flags = 0x00U
    };
    longDivision(&division, &result);
    unsigned long long returnValue = result.remainder;
    returnValue |= (unsigned long long) result.quotient.value << 32;  // NOLINT
    return returnValue;
}

#endif
