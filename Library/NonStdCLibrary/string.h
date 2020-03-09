//===-- string.h - Implementation of the string.h Standard Header -----------------------------------------*- C -*-===//
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
/// Support for several string-related functions. This header provides the interface to a subset of the C standard
/// <string.h> header and it's implementations.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_STRING_H
#define ZALEA_STRING_H

#include <stddef.h>

/// \brief a `memset` implementation compatible with the C standard `memset`.
///
/// This is an implementation of the C standard function `memset`. This function behaves exactly as the standard
/// version. Mostly, it implies that the fill is `int`, but only the first byte will be used as fill.
/// \param buffer this is a pointer that indicates where the buffer begins
/// \param fill this is the fill which will fill the buffer
/// \param size this is the size of the buffer to fill
/// \return the same address as provided in the buffer address
void *memset(void *buffer, int fill, size_t size);

/// \brief find whether a string is greater, smaller or equal to other string.
///
/// This is an implementation of the C standard function `strcmp`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or is a string without null
/// character at the end, then the behaviour is undefined.
/// \param string1 the string that will be compared.
/// \param string2 the string to compare.
/// \return &lt;0 if the first character that does not match is lower, 0 if both strings are equal and >0 if the first
/// character that does not match is greater.
int strcmp(const char *string1, const char *string2);

/// \brief find the length of a null-terminated string.
///
/// This is an implementation of the C standard function `strlen`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or is a string without null
/// character at the end, then the behaviour is undefined.
/// \param string is the null-terminated string to evaluate.
/// \return the size of the string, from the beginning to the null character without including it.
size_t strlen(const char *string);

#endif //ZALEA_STRING_H
