//===-- strtoullC.h - Implementation of `strtoull` (globally usable function) -----------------------------*- C -*-===//
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
/// Implementation of `strtoull` which is globally usable. This implementation can be used globally, and it's the base
/// of the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler
/// intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_STRTOULLC_H
#define ZALEA_STRTOULLC_H

#include <stdint.h>

/// If the sign bit flag is enabled, the number is positive.
#define SIGN_FLAG 0x00U

/// If the range bit flag is enabled, the converted value is out of range.
#define RANGE_FLAG 0x01U

/// If the base bit flag is enabled, the requested base is out of range.
#define BASE_FLAG 0x02U

/// \brief This special struct defines a number with flags.
///
/// All numbers in this struct are signed, but the value is unsigned and it's sign is defined by the sign bit in the
/// flags field. A special process is required to build a number from a *normal* number.
struct strtoullSignedT {
 /// The flags of this number. Flags are defined by the macros in this file.
 unsigned flags;
 /// The unsigned value of this number.
 uintmax_t value;
};

#endif //ZALEA_STRTOULLC_H
