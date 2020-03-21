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

/// \brief A `memcpy` implementation compatible with the C standard `memcpy`.
///
/// This is an implementation of the C standard function `memcpy`. This function behaves exactly as the standard
/// version. This means that it will yield undefined behaviour if the length overflows or if the combination of source,
/// destination and length overlaps.
///
/// \note This implementation is meant to be portable, not the fastest.
///
/// \param destination this is a pointer that indicates where the destination buffer begins.
/// \param source this is a pointer that indicates where the source buffer begins.
/// \param length the number of bytes to copy.
/// \return The same address as provided in the destination buffer.
void *memcpy(void *destination, const void *source, size_t length);

/// \brief A `memset` implementation compatible with the C standard `memset`.
///
/// This is an implementation of the C standard function `memset`. This function behaves exactly as the standard
/// version. It means that the fill is `int`, but only the first byte will be used as fill.
///
/// \note This implementation is meant to be portable, not the fastest.
///
/// \param buffer this is a pointer that indicates where the buffer begins.
/// \param fill this is the fill which will fill the buffer.
/// \param length this is the length of the buffer to fill.
/// \return The same address as provided in the buffer address.
void *memset(void *buffer, int fill, size_t length);

/// \brief Concatenate two C strings.
///
/// This is an implementation of the C standard function `strcat`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or with a string without a null
/// character at the end will yield undefined behaviour.
///
/// Undefined behaviour will be yield if the destination buffer is smaller than the concatenation or if the resulting
/// buffers overlap.
///
/// \param destination the string that will be compared.
/// \param source the string to compare.
/// \return The same address as provided in the destination address.
char *strcat(char *destination, const char *source);

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
int strcmp(const char *string1, const char *string2);

/// \brief Copy a the source string inside the destination string.
///
/// This is an implementation of the C standard function `strcpy`. This function behaves exactly as the standard
/// version. It means that calling this function with a pointer that is not a string or with a string without a null
/// character at the end will yield undefined behaviour.
///
/// Undefined behaviour will be yield if the destination buffer is smaller than the source buffer or if both buffers
/// overlap.
///
/// \param destination the destination buffer where to store the new copy of string.
/// \param source the source string that will be copied into the destination buffer.
/// \return The same address as provided in the destination address.
char *strcpy(char *destination, const char *source);

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
size_t strlen(const char *string);

#endif //ZALEA_STRING_H
