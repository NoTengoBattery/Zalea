//===-- rawStringToUnsignedPointer.c - An Implementation of the `stringToUnsignedPointer` Function --------*- C -*-===//
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
/// This is an implementation of `rawStringToUnsignedPointer` for the Non Standard C Library. This function is similar to
/// `strtoul`, but it will return an unsigned integer pointer instead. Use this function to convert a value which is
/// known to be a pointer to a `uintptr_t` value that can be used as a C pointer.
///
/// \note The two leading dashes is a way to differentiate this function from the one without them, because this
/// function is "raw", which means that it compiles with some features disabled while the real version will fully
/// compile with all features.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include "../CompilerSupport/CompilerRuntime/strtoullC.h"
#include "stdlib.h"
#include <CompilerMagic/BitwiseUtils.h>

uintptr_t rawStringToUnsignedPointer(const char *string, char **endingPointer, bool *range, bool *base, int radix) {
 if (range == NULL || base == NULL) { return 0x00U; }
 struct strtoullSignedT result = {0x00U, 0x00U};
 __strtoullC(string, endingPointer, (unsigned int) radix, &result, UINTPTR_MAX, 0x00U);
 *range = (bool) TEST_NTH_BIT(result.flags, RANGE_FLAG);
 *base = (bool) TEST_NTH_BIT(result.flags, BASE_FLAG);
 return (uintptr_t) result.value;
}
