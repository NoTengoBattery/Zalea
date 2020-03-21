//===-- AsciiUtils.h - Inline functions to Perform ASCII Operations -------------------------------------*- C++ -*-===//
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
/// operations with ASCII strings inside the C/C++ source code (which sometimes is not trivial to do).
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_ASCIIUTILS_H
#define ZALEA_ASCIIUTILS_H

#ifndef __ASSEMBLER__  // C and C++

#include <stdbool.h>  // NOLINT

/// \brief Check if the given character is an ASCII space character.
///
/// \param character the character to test.
/// \return true if the character is an ASCII space.
extern inline bool isAsciiSpace(const char character) {
 switch (character) {
  case '\t':
  case '\n':
  case '\v':
  case '\f':
  case '\r':
  case ' ':
   return true;
  default:
   return false;
 }
}

/// \brief Move a string pointer one character forward and return the character.
///
/// \param pointer the pointer to move.
/// \return The value of the character in the new position.
extern inline char moveStringPointerOneForward(const char **pointer) {
 *pointer += 0x01;
 return **pointer;
}

#ifdef __cplusplus  // C++

#endif

#else  // ASM

#endif

#endif //ZALEA_ASCIIUTILS_H
