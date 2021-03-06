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
/// the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stddef.h>

/// \brief Find the length of a null-terminated string.
///
/// This is an implementation of the C standard function `strlen`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or with a string without a null
/// character at the end will yield undefined behaviour.
///
/// \note This implementation is meant to be portable, not the fastest.
///
/// \param string is the null-terminated string to evaluate.
/// \return The size of the string, from the beginning to the null character without including it.
size_t __strlen(const char *string) {
 size_t length = 0x00;
 char character = *string;
 static const char nullChar = '\0';
 while (character != nullChar) {
  length += 0x01;
  string += 0x01;
  character = *string;
 }
 return length;
}
