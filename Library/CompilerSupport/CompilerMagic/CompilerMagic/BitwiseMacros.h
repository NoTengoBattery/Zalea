//===-- BitwiseMacros.h - Macros to Perform Bitwise Operations --------------------------------------------*- C -*-===//
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
/// The macros in this file will help the developers to easily and clearly modify and perform some bitwise operations
/// inside the C/C++ source code (which sometimes is not trivial to do).
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_BITWISEMACROS_H
#define ZALEA_BITWISEMACROS_H

#ifdef __cplusplus // C++

/// Set the n-th bit of a constant.
template<typename T1, typename T2>
constexpr auto clearNthBit(T1 x, T2 y) { return x & ~(1U << y); }

/// Clear the n-th bit of a constant.
template<typename T1, typename T2>
constexpr auto setNthBit(T1 x, T2 y) { return x | (1U << y); }

#elif defined(__ASSEMBLER__) // ASM


#endif

/// \brief Clear the n-th bit of a constant.
/// \param x the constant.
/// \param y the bit.
#define CLEAR_NTH_BIT(x, y) ((x) & ~(1U << y))
/// \brief Set the n-th bit of a constant.
/// \param x the constant.
/// \param y the bit.
#define SET_NTH_BIT(x, y) ((x) | (1U << y))
/// \brief Test the n-th bit of a constant.
/// \param x the constant.
/// \param y the bit.
#define TEST_NTH_BIT(x, y) (((x) >> y) & 1U)
/// \brief XOR then NOT the n-th bit of a constant.
/// \param x the constant.
/// \param y the constant.
/// \param z the bit.
#define XNOR_NTH_BITS(x, y, z) (TEST_NTH_BIT(x, z) == TEST_NTH_BIT(y, z) ? 0x01U : 0x00U)

#endif //ZALEA_BITWISEMACROS_H
