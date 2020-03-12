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
#include <string.h>

/// \brief Entry point from assembler to C.
///
/// This entry point is the secondary entry point for ARM. This is called directly from ASM, just after we have control
/// over the CPU. This will perform some testing and grab some information from the ARM ATAGS. However, since Linux had
/// deprecated the ATAGS, we cannot rely on it. We should get critical information from our implementation of the DTB
/// (not quite a DTB, just an equivalent).
/// \todo Add the required parameters from the bootloader to check them here.
void secondEntryPoint() ATTR_SECTION(".start");

ATTR_NORETURN void secondEntryPoint() {
    // Perform a small test of the Device Descriptor code... Please note that since it is a test, the property and it's
    // value are hardcoded. This should be the only special case of this.
    const char *testValue = getDeviceDescriptorProperty(deviceDescriptorTestProperty);
    if (!strcmp(testValue, deviceDescriptorTestValue)) {
        miserableFail();
    }
    memoryClear(&bssStart, &bssEnd);
    BUILTIN_UNREACHABLE;
}
