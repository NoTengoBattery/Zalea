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

/// \brief A generalized implementation of `strtoull`, which can be used to implement the other (strto... and ato...)
///
/// This function is a customized version. The standard-compatible versions can be used from the Non Standard C Library.
/// The main deference is that this function will take an struct pointer and use the provided struct to return the
/// result. This is because this function can convert an arbitrary string to an unsigned long long value with a sign,
/// which can be used to reinterpret the unsigned value as a signed value, and return it accordingly.
///
/// \note This implementation is inspired in the cplusplus.com/reference/cstdlib/strtol web page.
/// \note This implementation will not accept bases (radix) bigger than 36.
///
/// \param string a pointer to a C-like ASCII string which is to be interpreted.
/// \param endingPointer if not NULL, the pointer's value is changed to the address of the first invalid character.
/// \param base the base to use when performing the conversion.
/// \param result the struct that will be used to return the result of the conversion.
/// \param maximum the maximum value allowed for positive numbers (unsigned)
/// \param minimum the minimum value allowed for positive numbers (unsigned)
void __strtoullC(const char *string,
                 char **endingPointer,
                 unsigned base,
                 struct strtoullSignedT *result,
                 uintmax_t maximum,
                 uintmax_t minimum);

#endif //ZALEA_STRTOULLC_H
