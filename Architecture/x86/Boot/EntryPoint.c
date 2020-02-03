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

#include <CompilerMagic/CompilerMagic/CompilerMagic.h>
#include <ExecutableLibrary/ImageConstants.h>

/* We reject this value here, but we do the actual verification of everything in C++ */
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

/// This function will clear the allocated `.bss` section.
/// \param origin the origin of the memory buffer to set
/// \param final the target of the memory buffer to set
static inline void memoryClear(void *origin, void *final) ATTR_SECTION(".start");

/// This entry point is the secondary entry point for x86. As x86 implements (exclusively) the Multiboot2 protocol, this
/// function will perform some basic checking for the Multiboot2 i386 status as documented in the GNU GRUB user manual.
/// \param eax this is the value of the `EAX` register obtained from the bootloader
/// \param ebx this is the value of the `EBX` register obtained from the bootloader
void secondEntryPoint(unsigned int eax, unsigned int ebx) ATTR_SECTION(".start");

static inline void memoryClear(void *origin, void *final) {
    void *greater = ((origin > final) ? origin : final);
    void *smaller = ((origin < final) ? origin : final);
    if (greater != smaller) {
        unsigned char *buffer = smaller;
        while (buffer != greater) {
            *buffer = 0x00;
            buffer++;
        }
    }
}

ATTR_NORETURN void secondEntryPoint(unsigned int eax, unsigned int ebx) {
    if (eax != MULTIBOOT_2_BOOTLOADER_MAGIC  // If the magic value is not correct...
        || ((volatile void **) ebx >= &imageStart && (volatile void **) ebx <= &imageEnd)  // ... if inside the image...
        || ebx == 0x00) {  // ... or if it's pointer is null
        miserableFail();
        BUILTIN_UNREACHABLE;
    } else {
        // Note that clearing the .bss section will clear the stack, making all the frame pointers invalid.
        // Doesn't matter since these functions should never return, we only waste a couple of bytes of the stack.
        multibootStructPointer = (void *) ebx;  // NOLINT
        memoryClear(&bssStart, &bssEnd);
        BUILTIN_UNREACHABLE;
    }
}
