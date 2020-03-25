//===-- memset.c - Implementation of `memset` (globally usable function) ----------------------------------*- C -*-===//
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
/// Implementation of `memset` which is globally usable. This implementation can be used globally, and it's the base of
/// the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/BasesMagic.h>
#include <stddef.h>
#include <stdint.h>

/// \brief Loop byte by byte until the alignment requirement is fulfilled.
///
/// This small loop will fill unaligned memory in a byte-level loop and return the computed aligned address. It will
/// fill to either the required alignment or the maximum length.
///
/// \param buffer the buffer pointer.
/// \param fill the fill for the memory region.
/// \param length the length of the memory region.
/// \param alignment the expected alignment.
/// \return The address of the aligned buffer.
static inline size_t unalignedLoop(void *buffer, char fill, size_t length, unsigned alignment) {
 // Compute the buffer as a char addressing array...
 char *byteAddressing = buffer;
 // ... fill by char until either the length is depleted...
 while (length > 0x00) {
  unsigned modulo = (uintptr_t) byteAddressing % alignment;
  // ... or until the alignment is fulfilled...
  if (modulo == 0x00U && length >= alignment) { break; }
  *byteAddressing = fill;
  byteAddressing += 0x01;
  length -= 0x01;
 }
 // ... and the rest can be done in the unrolled loop
 return length;
}

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
/// \return The same address as provided in the buffer address or UINTPTR_MAX.
void *__memset(void *buffer, int fill, size_t length) {
 // If the length is 0, return immediately
 if (length == 0x00) { return buffer; }
 /*
  * We define a "cost". The cost is the size of the unrolled loop, which in theory should avoid branching and allow
  * the compiler to vectorize (if it the CPU has vector instructions).
  */
 char charFill = (char) fill;
 static const size_t cost = 0x08;
 static const size_t cellSize = sizeof(intmax_t);
 static const size_t alignment = cost * cellSize;
 size_t remainingSize = unalignedLoop(buffer, charFill, length, alignment);
 intmax_t *bigAddressing = (intmax_t *) ((uintptr_t) buffer + (length - remainingSize));
 if (remainingSize >= alignment) {
  // Create a variable of the appropriate length filled with the same value every byte
  intmax_t actualFill = 0x00;
  char *actualFillBytes = (char *) &actualFill;
  for (unsigned i = 0x00U; i < cellSize; ++i) { actualFillBytes[i] = (char) fill; }
  // The compiler will probably vectorize this loop :)
  do {
   bigAddressing[Zeroth] = actualFill;
   bigAddressing[First] = actualFill;
   bigAddressing[Second] = actualFill;
   bigAddressing[Third] = actualFill;
   bigAddressing[Fourth] = actualFill;
   bigAddressing[Fifth] = actualFill;
   bigAddressing[Sixth] = actualFill;
   bigAddressing[Seventh] = actualFill;
   bigAddressing += cost;
   remainingSize -= alignment;
  } while (remainingSize >= alignment);
 }
 size_t shouldBeZero = unalignedLoop(bigAddressing, charFill, remainingSize, alignment);
 return shouldBeZero != 0x00 ? (void *) UINTPTR_MAX : buffer;
}
