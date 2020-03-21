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

#include "division.h"
#include <CompilerMagic/BitwiseMacros.h>

void longDivision(struct divisionT *operands, struct resultT *result) {
 // Unpack the structs for better readability
 uintmax_t denominator = operands->denominator.value;
 unsigned denominatorFlags = operands->denominator.flags;
 uintmax_t numerator = operands->numerator.value;
 unsigned numeratorFlags = operands->numerator.flags;
 // Handle special cases ...
 //  1. Division by zero: quotient is -UINT_MAX, remainder is UINT_MAX and set the DIV0 flag of the quotient
 if (denominator == 0x00U) {
  result->quotient.value = UINTMAX_MAX;
  result->quotient.flags = SET_NTH_BIT(result->quotient.flags, DIV_0_FLAG);
  result->quotient.flags = CLEAR_NTH_BIT(result->quotient.flags, SIGN_FLAG);
  result->remainder = UINTMAX_MAX;
  return;
 }
 //  2. Division by one: the quotient is the number and the remainder is zero
 if (denominator == 0x01U) {
  result->quotient.value = numerator;
  result->quotient.flags = XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);
  result->remainder = 0x00U;
  return;
 }
 //  3. Division by itself: the quotient is one and the remainder is zero
 if (denominator == numerator) {
  result->quotient.value = 0x01U;
  result->quotient.flags = SET_NTH_BIT(result->quotient.flags, SIGN_FLAG);;
  result->remainder = 0x00U;
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
 uintmax_t shiftedDenominator = denominator;
 unsigned steps = 0x00U;
 while (shiftedDenominator <= numerator) {
  shiftedDenominator = shiftedDenominator << 0x01U;
  steps += 0x01U;
 }
 shiftedDenominator = shiftedDenominator >> 0x01U;
 // Do the long division based on the shifted denominator calculated above
 uintmax_t quotient = 0x00U;
 uintmax_t remainder = numerator;
 for (unsigned i = 0x00; i < steps; ++i) {
  if (remainder >= shiftedDenominator) {
   remainder -= shiftedDenominator;
   quotient = quotient << 0x01U;
   quotient += 0x01U;
  } else { quotient = quotient << 0x01U; }
  shiftedDenominator = shiftedDenominator >> 0x01U;
 }
 // Set the struct values and return
 result->quotient.value = quotient;
 result->quotient.flags = XNOR_NTH_BITS(numeratorFlags, denominatorFlags, SIGN_FLAG);
 result->remainder = remainder;
}
