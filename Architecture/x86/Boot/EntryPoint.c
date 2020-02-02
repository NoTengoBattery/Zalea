//===-- EntryPoint.c - C Entry Point ----------------------------------------------------------------------*- C -*-===//
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
/// This entry point will be called from the very first boot code (from 'start', written in ASM) and will setup the
/// environment to further start the C++ code.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/CompilerMagic.h>
#include <ImageConstants.h>
#include <stdint.h>

/* We reject this value here, but we do the actual verification of everything in C++ */
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

volatile void *multibootStructPointer = 0x00;

/// fakeMemset4 is a "fake" memset made out of 4 consecutive sets to 0 of unsigned integers of 64 bit. This function
/// exists because the compiler will try to use 'memset' if it don't. We will not depend on another "library" of this
/// project, except from CompilerMagic. Therefore, we should ad-hoc implement anything we need.
/// \param buffer this is the base buffer, it's volatile to avoid compiler optimization
/// \param value the value that will fill the memory region
/// \return the updated buffer pointer value after the operation
uint64_t *fakeMemset4(volatile uint64_t *buffer, uint64_t value) ATTR_SECTION(".start");

/// memoryClear464 will clear an aligned block of memory in 4 operations of 64 bit set. The block must have to be
/// aligned to 64*4 bits (256 bits). Otherwise, the function will return immediately.
/// \param origin the origin of the memory buffer to set
/// \param final the target of the memory buffer to set
void memoryClear464(void *origin, void *final) ATTR_SECTION(".start");

/// This entry point is the secondary entry point for x86. As x86 implements (exclusively) the Multiboot2 protocol, this
/// function will perform some basic checking for the Multiboot2 i386 status as documented in the GNU GRUB user manual.
/// \param eax this is the value of the EAX register obtained from the bootloader
/// \param ebx this is the value of the EBX register obtained from the bootloader
void secondEntryPoint(unsigned int eax, unsigned int ebx) ATTR_SECTION(".start");

inline void memoryClear464(void *origin, void *final) {
    volatile void *greater = ((origin > final) ? origin : final);
    volatile void *smaller = ((origin < final) ? origin : final);
    const unsigned int numberOfOperations = 4;
    if (greater == smaller  // No action required
        || (unsigned int) smaller % (numberOfOperations * sizeof(uint64_t)) != 0  // Origin is not aligned
        || (unsigned int) greater % (numberOfOperations * sizeof(uint64_t)) != 0) {  // Destination is not aligned
        return;
    }
    uint64_t *buffer = (uint64_t *) smaller;  // NOLINT
    while ((volatile void *) buffer != greater) {
        buffer = fakeMemset4(buffer, 0x00);
    }
}

inline uint64_t *fakeMemset4(volatile uint64_t *buffer, uint64_t value) {
    buffer[0] = value;
    buffer[1] = value;
    buffer[2] = value;
    buffer[3] = value;
    REORDERING_BARRIER;
    return (uint64_t *) (buffer + 4);  // NOLINT
}

ATTR_NORETURN void secondEntryPoint(unsigned int eax, unsigned int ebx) {  // NOLINT
    if (eax != MULTIBOOT_2_BOOTLOADER_MAGIC  // If the magic value is not correct...
        || ((volatile void **) ebx >= &imageStart && (volatile void **) ebx <= &imageEnd)  // ... if inside the image...
        || ebx == 0x00) {  // ... or if it's pointer is null
        miserableFail();
        BUILTIN_UNREACHABLE;
    } else {
        // Because the pointer is stored in bss, we store it after we clear the bss and stack
        memoryClear464(&bssStart, &imageEnd);
        multibootStructPointer = (void *) ebx;  // NOLINT
    }
    BUILTIN_UNREACHABLE;
}
