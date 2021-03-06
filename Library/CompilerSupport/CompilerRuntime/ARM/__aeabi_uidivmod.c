//===-- __aeabi_uidivmod.c - ARM EABI Unsigned Integer Division with Modulo -------------------------------*- C -*-===//
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
/// Implement the `__aeabi_uidivmod` support function.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include "ArmUtils.h"
#include <CompilerMagic/BitwiseUtils.h>
#include <CompilerMagic/CompilerMagic.h>
#include <config.h>
#include <division.h>

#if defined(KERNEL_ARM) && defined(KERNEL_COMPILER_GNU)

/// \brief This is the EABI call for unsigned integer division with quotient.
///
/// In theory, the return quotient will go on R0 and the remainder in R1, but we can't control that (directly) from C.
/// But the ABI says that a 64 bit value will, indeed, be returned that way, so we return a 64 bit struct which is the
/// composition of both the remainder and quotient. This should work as long as EABI is used (and this function won't be
/// called if you don't use EABI, anyway).
///
/// \param numerator (parameter introduced by the compiler).
/// \param denominator (parameter introduced by the compiler).
/// \return The composite value of the results which conforms with the EABI call.
ATTR_USED uint64_t __aeabi_uidivmod(uint32_t numerator, uint32_t denominator) {
 struct divisionT division = {
   .denominator.flags = SET_NTH_BIT(0x00U, SIGN_FLAG),
   .denominator.value = denominator,
   .numerator.flags =  SET_NTH_BIT(0x00U, SIGN_FLAG),
   .numerator.value = numerator
 };
 struct resultT result = {
   .quotient.flags = 0x00U,
   .quotient.value = 0x00U,
   .remainder = 0x00U
 };
 longDivision(&division, &result);
 extern uint64_t __arm_noop(uint32_t, uint32_t);
 return __arm_noop(result.quotient.value, result.remainder);
}

#endif
