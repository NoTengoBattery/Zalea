//===-- ArithmeticUtils.h - Inline functions to Perform Arithmetic Operations ---------------------------*- C++ -*-===//
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
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// The inline functions and macros in this file will help the developers to easily and clearly modify and perform some
/// operations with integer values inside the C/C++ source code (which sometimes is not trivial to do).
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_ARITHMETICUTILS_H
#define ZALEA_ARITHMETICUTILS_H

#ifndef __ASSEMBLER__  // C and C++

#include <stdbool.h>  // NOLINT
#include <stdint.h>  // NOLINT

/// \brief Perform a "safe" unsigned addition, which will not wrap around.
///
/// Perform a "safe" unsigned addition of two numbers, given a maximum value, that will not wrap around. Instead of
/// wrapping around, it will clamp the value to the maximum requested by the caller and return true to indicate an
/// overflow.
///
/// \param a the first number to add.
/// \param b the second number to add.
/// \param max the maximum value to allow.
/// \param r a pointer to a variable where the result will be stored.
/// \return true if the addition does overflow, and this will set the result to the maximum value; false otherwise.
extern inline bool safeUnsignedAddition(uintmax_t a, uintmax_t b, uintmax_t max, uintmax_t *r) {
 if (a > max - b) {
  *r = max;
  return true;
 }
 *r = a + b;
 return false;
}

/// \brief Perform a "safe" unsigned multiplication, which will not wrap around.
///
/// Perform a "safe" unsigned multiplication of two numbers, given a maximum value, that will not wrap around. Instead
/// of wrapping around, it will clamp the value to the maximum requested by the caller and return true to indicate an
/// overflow.
///
/// \param a the first number to multiply.
/// \param b the second number to multiply.
/// \param max the maximum value to allow.
/// \param r a pointer to a variable where the result will be stored.
/// \return true if the multiplication does overflow which will set the result to the maximum value; false otherwise.
extern inline bool safeUnsignedMultiplication(uintmax_t a, uintmax_t b, uintmax_t max, uintmax_t *r) {
 if ((b != 0x00 && a > max / b) ||
   (a != 0x00 && b > max / a)) {
  *r = max;
  return true;
 }
 *r = a * b;
 return false;
}

#ifdef __cplusplus  // C++

#endif

#else  // ASM

#endif

#endif //ZALEA_ARITHMETICUTILS_H
