//===-- strcmp.c - Implementation of the strcmp Standard Function -----------------------------------------*- C -*-===//
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
/// Implement the strcmp function.
///
//===--------------------------------------------------------------------------------------------------------------===//

// This is a trivial strcmp implementation, the non-standard implementation maybe will be faster. Or maybe the compiler
// can optimize something more.
int strcmp(const char *string1, const char *string2) {
    const unsigned char *pointer1 = (const unsigned char *) string1;
    const unsigned char *pointer2 = (const unsigned char *) string2;
    while (*pointer1 && *pointer1 == *pointer2) {
        ++pointer1, ++pointer2;
    }
    return (*pointer1 > *pointer2) - (*pointer2 > *pointer1);
}
