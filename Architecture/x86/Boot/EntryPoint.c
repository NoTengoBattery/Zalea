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
/// environment to further start the C++ code. This file will setup some basic and critical environment features,
/// perform some crude tests and then continue to the rest of the boot code once done.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/CompilerMagic.h>
#include <DeviceDescriptor.h>
#include <ExecutableLibrary/ImageConstants.h>
#include <InlineMagic/MemoryClear.h>

/// \brief Entry point from assembler to C.
///
/// This entry point is the secondary entry point for x86. As x86 implements (exclusively) the Multiboot2 protocol, this
/// function will perform some basic checking for the Multiboot2 i386 status as documented in the GNU GRUB user manual.
/// \param magic this is the special magic number that the bootloader should provide.
/// \param mbs this is the pointer to the structure of information that the bootloader should provide.
void secondEntryPoint(unsigned int magic, unsigned int mbs) ATTR_SECTION(".start");

ATTR_NORETURN void secondEntryPoint(unsigned int magic, unsigned int mbs) {
    if (magic != multibootMagicConstant  // If the magic value is not correct...
        || ((unsigned *) mbs >= &imageStart && (unsigned *) mbs <= &imageEnd)  // ... if inside the image...
        || (void *) mbs == NULL) {  // ... or if it's pointer is null
        miserableFail();
        BUILTIN_UNREACHABLE;
    } else {
        // If the Device Descriptor lookup does not work, terminate the execution immediately
        if (isDeviceDescriptorWorking() == false) { miserableFail(); }
        // Store the value of EAX inside the (temporary) Multiboot pointer
        *multibootStructPointer = (volatile void *) mbs;
        // Clear the BSS section of the loaded memory...
        memoryClear(&bssStart, &bssEnd);
        BUILTIN_UNREACHABLE;
    }
}
