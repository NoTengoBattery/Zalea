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
#include <division.h>
#include <limits.h>

void longDivision(struct divisionT *operands, struct resultT *result) {
    // Unpack the structs for better readability
    unsigned denominator = operands->denominator.value;
    unsigned denominatorFlags = operands->denominator.flags;
    unsigned numerator = operands->numerator.value;
    unsigned numeratorFlags = operands->numerator.flags;
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
        result->quotient.flags = XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);
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
        result->quotient.flags = XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);
        result->remainder = numerator;
        return;
    }
    // Shift the denominator until it's bigger or equal than the numerator
    unsigned shiftedDenominator = denominator;
    unsigned int steps = 0x00U;
    while (shiftedDenominator <= numerator) {
        shiftedDenominator = shiftedDenominator << 0x01U;
        steps += 1;
    }
    shiftedDenominator = shiftedDenominator >> 0x01U;
    // Do the long division based on the shifted denominator calculated above
    unsigned quotient = 0x00U;
    unsigned remainder = numerator;
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
    result->quotient.flags = XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);
    result->remainder = remainder;
}
