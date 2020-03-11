//===-- strlen.c - Implementation of the strlen Standard Function -----------------------------------------*- C -*-===//
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
/// Implement the strlen function.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stddef.h>

// This is a trivial strlen implementation, the non-standard implementation maybe will be faster. Or maybe the compiler
// can optimize something more.
size_t strlen(const char *string) {
    size_t length = 0;
    while (*string != 0x00) {
        length += 1;
        string += 1;
    }
    return length;
}
