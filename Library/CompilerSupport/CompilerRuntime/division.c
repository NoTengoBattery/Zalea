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
#include <config.h>
#include <limits.h>

/// If the sign flag is enabled, the number is positive
#define SIGN_FLAG 0x00U
/// If the div0 flag is enabled, a division by zero has occurred
#define DIV0_FLAG 0x01U

struct signedT {
    unsigned char flags;
    unsigned long value;
};

struct divisionT {
    struct signedT numerator;
    struct signedT denominator;
};

struct resultT {
    unsigned long remainder; // Remainder is always positive or 0, by definition
    struct signedT quotient; // Use the struct to recycle the signed division for unsigned values discarding the sign
};

static inline unsigned char xnorBits(unsigned char val1, unsigned char val2, unsigned char bit) {
    unsigned char val1Bit = TEST_NTH_BIT(val1, bit);
    unsigned char val2Bit = TEST_NTH_BIT(val2, bit);
    return (val1Bit == val2Bit ? 0x01U : 0x00U);
}

ATTR_USED static inline void longDivision(struct divisionT *operands, struct resultT *result) {
    // Unpack the structs for better readability
    unsigned long denominator = operands->denominator.value;
    unsigned char denominatorFlags = operands->denominator.flags;
    unsigned long numerator = operands->numerator.value;
    unsigned char numeratorFlags = operands->numerator.flags;
    // Handle special cases ...
    //  1. Division by zero: return q = ULONG_MAX, r = 0 and set the DIV0 flag of the quotient
    if (denominator == 0x00U) {
        result->quotient.value = ULONG_MAX;
        result->quotient.flags = SET_NTH_BIT(0x00U, DIV0_FLAG);
        result->remainder = 0x00UL;
        return;
    }
    //  2. Division by one: the quotient is the number and the remainder is zero
    if (denominator == 0x01U) {
        result->quotient.value = numerator;
        result->quotient.flags = xnorBits(numeratorFlags, denominatorFlags, SIGN_FLAG);
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
    //  3. Division by a larger denominator: the quotient is zero and the remainder is the numerator
    if (denominator > numerator) {
        result->quotient.value = 0x00U;
        result->quotient.flags = xnorBits(numeratorFlags, denominatorFlags, SIGN_FLAG);
        result->remainder = numerator;
        return;
    }
    // Find the numerator most significant bit (it will help us to align the denominator)
    unsigned long shiftedNumerator = numerator;
    unsigned int numeratorMostSignificantBit = 0x00;
    while (shiftedNumerator >> 0x01U != 0) {
        shiftedNumerator = shiftedNumerator >> 0x01U;
        numeratorMostSignificantBit += 1;
    }
    // Align the denominator to the most significant bit
    unsigned long shiftedDenominator = denominator;
    unsigned int steps = 0x01U;
    while (((shiftedDenominator >> numeratorMostSignificantBit) & 0x01U) == 0) {
        shiftedDenominator = shiftedDenominator << 0x01U;
        steps += 1;
    }
    // Do the long division based on the steps calculated before
    unsigned long quotient = 0x00;
    unsigned long remainder = numerator;
    for (unsigned int i = 0; i < steps; ++i) {
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
    result->quotient.flags = xnorBits(numeratorFlags, denominatorFlags, SIGN_FLAG);
    result->remainder = remainder;
}

#if defined(KERNEL_ARM) && defined(KERNEL_COMPILER_GNU)

unsigned long long __aeabi_uidivmod(unsigned long numerator, unsigned long denominator) {
    struct divisionT division = {
            .denominator.flags = 0x01U,
            .denominator.value = denominator,
            .numerator.flags = 0x01U,
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
