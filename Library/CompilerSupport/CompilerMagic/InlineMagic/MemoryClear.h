//===-- MemoryClear.h - Static Inline Function to Clear Memory --------------------------------------------*- C -*-===//
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
/// A small function to clear a region of memory. This function is static inline, as is meant to be inserted by the
/// compiler instead of being called.
///
//===--------------------------------------------------------------------------------------------------------------===//
#ifndef ZALEA_MEMORYCLEAR_H
#define ZALEA_MEMORYCLEAR_H

#include <stddef.h>
#include <stdint.h>

/// \brief This function will clear memory region between two pointers.
///
/// The compiler will either call *memset* or may vectorize this function. This function is inline, so it will replace
/// any other implementation. The compiler may optimize this by inserting the code instead of calling.
/// \param origin the origin of the memory buffer to set.
/// \param final the target of the memory buffer to set.
inline void memoryClear(void *origin, void *final);

extern inline void memoryClear(void *origin, void *final) {
    void *greater = ((origin > final) ? origin : final);
    void *smaller = ((origin < final) ? origin : final);
    if (greater != smaller) {
        unsigned char *buffer = smaller;
        size_t size = (uintptr_t) greater - (uintptr_t) smaller;
        while (size != 0) {
            buffer[0] = 0x00;
            buffer += 1;
            size -= 1;
        }
    }
}

#endif //ZALEA_MEMORYCLEAR_H
