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

#define CLEAR_NTH_BIT(x, y) ((x) & ~(1U << y ## U))
#define SET_NTH_BIT(x, y) ((x) | (1U << y ## U))

#endif //ZALEA_BITWISEMACROS_H
