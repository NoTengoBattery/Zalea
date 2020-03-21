//===-- strcmp.c - Implementation of `strcmp` (globally usable function) ----------------------------------*- C -*-===//
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
/// Implementation of `strcmp` which is globally usable. This implementation can be used globally, and it's the base of
/// the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

/// \brief Find whether a string is greater, smaller or equal to other string.
///
/// This is an implementation of the C standard function `strcmp`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or with a string without a null
/// character at the end will yield undefined behaviour.
///
/// \note This implementation is meant to be portable, not the fastest.
///
/// \param string1 the string that will be compared.
/// \param string2 the string to compare.
/// \return &lt;0 if the first character that does not match is lower, &#61;0 if both strings are equal and &gt;0 if the
/// first character that does not match is greater.
int __strcmp(const char *string1, const char *string2) {
 char char1 = *string1;
 char char2 = *string2;
 static const char nullChar = '\0';
 if (string1 != string2) {  // Shortcut: two strings are equal if they point to the same address.
  while (char1 == char2 && char1 != nullChar && char2 != nullChar) {
   string1 += 0x01;
   char1 = *string1;
   string2 += 0x01;
   char2 = *string2;
  }
  return char1 - char2;
 }
 return 0x00;
}
