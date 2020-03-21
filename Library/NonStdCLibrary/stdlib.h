//===-- stdlib.h - Implementation of the stdlib.h Standard Header -----------------------------------------*- C -*-===//
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

#ifndef ZALEA_STDLIB_H
#define ZALEA_STDLIB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/// \brief an implementation of a `strtoul`-like function that returns an unsigned integer pointer instead.
///
/// This function behaves like the `strtoul` standard function, but it does return an unsigned integer pointer instead
/// of a unsigned long. Most likely, the machine's `uintptr_t` is in fact a unsigned long, but you should not rely on
/// it.
///
/// \param string a string to convert into a unsigned pointer.
/// \param endingPointer a pointer to a pointer which will hold the address of the first non-convertible character.
/// \param range a pointer to a bool that will be true if the value is out of range, and false otherwise.
/// \param radix the base which represents the encoded string as a valid number.
/// \param base a pointer to a bool that will be true if the base is out of range, and false otherwise.
/// \attention endingPointer can be NULL.
/// \attention range cannot be NULL.
/// \attention base cannot be NULL.
/// \return The value computed from the string. If the string cannot be converted, it will return NULL. If the string is
/// out of range, it will return UINTPTR_MAX. The `range` parameter will be true if the conversion resulted in an out
/// of range value, given the maximum pointer that `uintptr_t` can hold.
uintptr_t stringToUnsignedPointer(const char *string, char **endingPointer, bool *range, bool *base, int radix);

#endif //ZALEA_STDLIB_H
