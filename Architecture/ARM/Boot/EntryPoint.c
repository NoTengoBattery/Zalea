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

#include <CompilerMagic/BasesMagic.h>
#include <CompilerMagic/CompilerMagic.h>
#include <DeviceDescriptor.h>
#include <ExecutableLibrary/ImageConstants.h>
#include <InlineMagic/MemoryClear.h>
#include <stdlib.h>

/// \brief Entry point from assembler to C.
///
/// This entry point is the secondary entry point for ARM. This is called directly from ASM, just after we have control
/// over the CPU. This will perform some testing and grab some information from the ARM ATAGS. However, since Linux had
/// deprecated the ATAGS, we cannot rely on it. We should get critical information from our implementation of the DTB
/// (not quite a DTB, just an equivalent).
///
/// \param machine the machine code as returned by the bootloader.
/// \param atags the address to the ATAGS as returned by the bootloader.
void secondEntryPoint(unsigned machine, unsigned atags)
ATTR_SECTION(".start");

ATTR_NORETURN void secondEntryPoint(unsigned machine, unsigned atags) {
 if ((void *) atags == NULL  // If the ATAGS are null...
   || ((char *) atags >= &imageStart && (char *) atags <= &imageEnd)) {  // ... or if inside the image...
  miserableFail();
  BUILTIN_UNREACHABLE;
 } else {
  // If the Device Descriptor lookup does not work, terminate the execution immediately
  if (isDeviceDescriptorWorking() == false) { miserableFail(); }
  // Store the value of ATAGS inside the (temporary) ATAGS pointer
  *atagsStructPointer = (volatile void *) atags;
  // Store the machine code (to be further processed later)
  *armMachineCode = machine;
  // Clear the BSS section of the loaded memory...
  memoryClear(&bssStart, &bssEnd);
  // Try to find the address of the vector table for the processor (to copy it from the binary image)
  const char *vectorTablePosition = getDeviceDescriptorProperty("interrupt" KS "vector" KS "default");
  const size_t positionLength = strlen(vectorTablePosition) + 0x01;
  const char *vectorTableAddressPrefix = "interrupt" KS "vector" KS;
  const size_t addressPrefixLength = strlen(vectorTableAddressPrefix) + 0x01;
  char buffer[positionLength + addressPrefixLength];
  strcpy(buffer, vectorTableAddressPrefix);  // NOLINT
  strcat(buffer, vectorTablePosition);  // NOLINT
  bool range;
  bool base;
  // Once we have the address of destination, copy the vector table from the image to it's destination
  const char *vectorTable = getDeviceDescriptorProperty(buffer);
  uintptr_t tableAddress = __stringToUnsignedPointer(vectorTable, NULL, &range, &base, HexadecimalBase);
  size_t vectorTableLength = vecend - &imageStart;
  memcpy((void *) tableAddress, &imageStart, vectorTableLength);  // NOLINT
  BUILTIN_UNREACHABLE;
 }
}
