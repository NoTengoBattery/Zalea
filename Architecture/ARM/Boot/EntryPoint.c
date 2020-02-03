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

/// This function will clear the allocated `.bss` section.
/// \param origin the origin of the memory buffer to set
/// \param final the target of the memory buffer to set
static inline void memoryClear(void *origin, void *final) ATTR_SECTION(".start");

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

ATTR_NORETURN void secondEntryPoint() {
    memoryClear(&bssStart, &bssEnd);
    BUILTIN_UNREACHABLE;
}
