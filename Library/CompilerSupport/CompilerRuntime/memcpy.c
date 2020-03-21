//===-- memcpy.c - Implementation of `memcpy` (globally usable function) ----------------------------------*- C -*-===//
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
/// Implementation of `memcpy` which is globally usable. This implementation can be used globally, and it's the base of
/// the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

/// \brief loop byte by byte until the alignment requirement is fulfilled.
///
/// This small loop will copy unaligned memory in a byte-level loop and return the computed aligned address. It will
/// fill to either the required alignment or the maximum size.
///
/// \note This function will try to align both buffer addresses, by moving both at the same time, byte by byte. This
/// means that, if somehow, the addresses cannot be aligned at the same time by iterating them together, the full
/// length will be copied byte by byte. This will cause severe performance loss.
///
/// \param destination the buffer pointer.
/// \param source the fill for the memory region.
/// \param length the length of the memory region.
/// \param alignment the expected alignment.
/// \return The address of the aligned buffer.
static inline size_t unalignedLoop(void *destination, const void *source, size_t length, unsigned alignment) {
 // Compute the buffers as a char addressing array...
 char *byteDestinationAddressing = destination;
 const char *byteSourceAddressing = source;
 // ... copy by char until either the length is depleted...
 while (length > 0x00) {
  unsigned destinationModuli = (uintptr_t) byteDestinationAddressing % alignment;
  unsigned sourceModuli = (uintptr_t) byteSourceAddressing % alignment;
  // ... or until the alignment is fulfilled...
  if (destinationModuli == 0x00 && sourceModuli == 0x00 && length >= alignment) { break; }
  *byteDestinationAddressing = *byteSourceAddressing;
  byteDestinationAddressing += 0x01;
  byteSourceAddressing += 0x01;
  length -= 0x01;
 }
 // ... and the rest can be done in the unrolled loop
 return length;
}

/// \brief a `memcpy` implementation compatible with the C standard `memcpy`.
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
void *__memcpy(void *destination, const void *source, size_t length) {
 // If the length is 0, return immediately
 if (length == 0x00) { return destination; }
 /*
  * We define a "cost". The cost is the size of the unrolled loop, which in theory should avoid branching and allow
  * the compiler to vectorize (if it the CPU has vector instructions).
  */
 static const size_t cost = 0x08;
 static const size_t cellSize = sizeof(intmax_t);
 static const size_t alignment = cost * cellSize;
 size_t remainingSize = unalignedLoop(destination, source, length, alignment);
 intmax_t *wordDestinationAddressing = (intmax_t *) ((uintptr_t) destination + (length - remainingSize));
 const intmax_t *wordSourceAddressing = (const intmax_t *) ((uintptr_t) source + (length - remainingSize));
 // The compiler will probably vectorize this loop :)
 while (remainingSize >= alignment) {
  wordDestinationAddressing[0] = wordSourceAddressing[0];
  wordDestinationAddressing[1] = wordSourceAddressing[1];
  wordDestinationAddressing[2] = wordSourceAddressing[2];
  wordDestinationAddressing[3] = wordSourceAddressing[3];
  wordDestinationAddressing[4] = wordSourceAddressing[4];
  wordDestinationAddressing[5] = wordSourceAddressing[5];  // NOLINT
  wordDestinationAddressing[6] = wordSourceAddressing[6];  // NOLINT
  wordDestinationAddressing[7] = wordSourceAddressing[7];  // NOLINT
  wordDestinationAddressing += cost;
  wordSourceAddressing += cost;
  remainingSize -= alignment;
 }
 size_t shouldBeZero = unalignedLoop(wordDestinationAddressing, wordSourceAddressing, remainingSize, alignment);
 return shouldBeZero != 0x00 ? (void *) UINTPTR_MAX : destination;
}
