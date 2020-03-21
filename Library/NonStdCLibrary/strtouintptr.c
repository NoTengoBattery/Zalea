//===-- strtouintptr.c - An Implementation of the `stringToUnsignedPointer` C Non-Standard Function -------*- C -*-===//
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
/// This is an implementation of `stringToUnsignedPointer` for the Non Standard C Library. This function is similar to
/// `strtoul`, but it will return an unsigned integer pointer instead. Use this function to convert a value which is
/// known to be a pointer to a `uintptr_t` value that can be used as a C pointer.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include "../CompilerSupport/CompilerRuntime/strtoullC.h"
#include "stdlib.h"
#include <CompilerMagic/BitwiseMacros.h>

uintptr_t stringToUnsignedPointer(const char *string, char **endingPointer, bool *range, bool *base, unsigned radix) {
 if (range == NULL || base == NULL) { return 0x00; }
 struct strtoullSignedT result;
 __strtoullC(string, endingPointer, radix, &result, UINTPTR_MAX, 0x00U);
 bool sign = TEST_NTH_BIT(result.flags, SIGN_FLAG);
 *range = TEST_NTH_BIT(result.flags, RANGE_FLAG);
 *base = TEST_NTH_BIT(result.flags, BASE_FLAG);
 return sign ? result.value : -result.value;
}
