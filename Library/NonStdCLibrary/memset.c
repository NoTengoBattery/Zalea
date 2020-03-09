//===-- memset.c - An Implementation of 'memset' for The Compiler Runtime ---------------------------------*- C -*-===//
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
/// This is an implementation of 'memset' for the compiler runtime. The compiler, some times, will detect a memset-like
/// behaviour and will replace the code with a memset call. This function does not expose an API, therefore, it can't be
/// used directly.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/CompilerMagic.h>
#include <stddef.h>
#include <stdint.h>

/// \brief loop byte by byte until the alignment requirement is fulfilled.
///
/// This small loop will fill unaligned memory in a byte-level loop and return the computed aligned address. It will
/// fill to either the required alignment or the maximum size.
/// \param buffer the buffer pointer
/// \param fill the fill for the memory region
/// \param size the size of the memory region
/// \param alignment the expected alignment
/// \return the address of the aligned buffer
ATTR_ALWAYS_INLINE
static inline size_t unalignedLoop(void *buffer, unsigned char fill, size_t size, unsigned int alignment) {
    // Compute the buffer as a char addressing array...
    unsigned char *byteAddressing = buffer;
    // ... fill by char until either the size is depleted...
    while (size > 0) {
        // ... or until the alignment is fulfilled...
        if ((uintptr_t) byteAddressing % alignment == 0) {
            // ... and the rest can be done in the unrolled loop
            if (size >= alignment) {
                break;
            }
        }
        *byteAddressing = fill;
        byteAddressing += 1;
        size -= 1;
    }
    return size;
}

void *memset(void *buffer, int fill, size_t size) {
    // If the size is 0, return immediately
    if (size == 0) { return buffer; }
    // We define a "cost". The cost is the size of the unrolled loop, which in theory should avoid branching...
    static const size_t cost = 4;
    static const size_t cellSize = sizeof(uint_fast64_t);
    static const size_t alignment = cost * cellSize;
    size_t remainingSize = unalignedLoop(buffer, (unsigned char) fill, size, alignment);
    uint_fast64_t *wordAddressing = (uint_fast64_t *) ((uintptr_t) buffer + size - remainingSize);
    if (remainingSize >= alignment) {
        // Create a variable of the appropriate size filled with the same value every byte
        uint_fast64_t actualFill = (uint_fast64_t) fill;
        for (unsigned int i = 0x00; i < cellSize; ++i) {
            ((unsigned char *) &actualFill)[i] = (unsigned char) fill;
        }
        // The compiler will probably vectorize this loop :)
        while (remainingSize >= alignment) {
            wordAddressing[0] = actualFill;
            wordAddressing[1] = actualFill;
            wordAddressing[2] = actualFill;
            wordAddressing[3] = actualFill;
            wordAddressing += cost;
            remainingSize -= alignment;
        }
    }
    size_t shouldBeZero = unalignedLoop(wordAddressing, (unsigned char) fill, remainingSize, alignment);
    if (shouldBeZero != 0) {
        return (void *) UINTPTR_MAX;
    }
    return buffer;
}