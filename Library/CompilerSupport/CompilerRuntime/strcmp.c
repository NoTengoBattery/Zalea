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
/// the Non Standard C Library and it's part of the Compiler Runtime, which means that it is similar to a compiler
/// intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

/// \brief find whether a string is greater, smaller or equal to other string.
///
/// This is an implementation of the C standard function `strcmp`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or is a string without null
/// character at the end, then the behaviour is undefined.
/// \param string1 the string that will be compared.
/// \param string2 the string to compare.
/// \return &lt;0 if the first character that does not match is lower, 0 if both strings are equal and >0 if the first
/// character that does not match is greater.
int __strcmp(const char *string1, const char *string2) {
    unsigned char value1 = (unsigned char) *string1;
    unsigned char value2 = (unsigned char) *string2;
    if (string1 != string2) {
        while (value1 == value2 && value1 != 0x00 && value2 != 0x00) {
            string1 += 0x01;
            string2 += 0x01;
            value1 = (unsigned char) *string1;
            value2 = (unsigned char) *string2;
        }
        return value1 - value2;
    }
    return 0x00;
}
