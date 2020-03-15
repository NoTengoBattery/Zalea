//===-- strlen.c - Implementation of `strlen` (globally usable function) ----------------------------------*- C -*-===//
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
/// Implementation of `strlen` which is globally usable. This implementation can be used globally, and it's the base of
/// the Non Standard C Library and it's part of the Compiler Runtime, which means that it is similar to a compiler
/// intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stddef.h>

/// \brief find the length of a null-terminated string.
///
/// This is an implementation of the C standard function `strlen`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or is a string without null
/// character at the end, then the behaviour is undefined.
/// \param string is the null-terminated string to evaluate.
/// \return the size of the string, from the beginning to the null character without including it.
size_t __strlen(const char *string) {
    size_t length = 0;
    while (*string != 0x00) {
        length += 1;
        string += 1;
    }
    return length;
}
