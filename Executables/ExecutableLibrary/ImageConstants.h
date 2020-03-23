//===-- ImageConstants.h - Link-Time Constant Addresses of the Binary Image -------------------------------*- C -*-===//
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
/// All of these constants are pointers that live inside the final binary image. They are constants at link and build
/// time. Also, some other relevant constants that need to be globally accessible.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_IMAGECONSTANTS_H
#define ZALEA_IMAGECONSTANTS_H

#include <stdint.h>  // NOLINT

/// \brief This is a magic number which is expected to be present in the Multiboot2 i386 Machine State.
#define MULTIBOOT_MAGIC_CONSTANT 0x36D76289  // NOLINT

/// \brief This is a pointer to the end of the bss section.
extern char bssEnd;
/// \brief This is a pointer to the start of the bss section.
extern char bssStart;
/// \brief This is a pointer to the end of the binary image.
extern char imageEnd;
/// \brief This is a pointer to the start of the binary image.
extern char imageStart;

/// \brief This is a pointer that will be the Multiboot2 struct as returned by the bootloader.
extern volatile void **multibootStructPointer;
/// \brief This is a pointer that will be the ATAGS struct as returned by the bootloader.
extern volatile void **atagsStructPointer;
/// \brief This is a pointer that will be the ARM Machine code as returned by the bootloader.
extern volatile unsigned *armMachineCode;
/// \brief This is a pointer to the end of the ARM vector table, which needs to be copied into memory.
extern char *vecend;

/// \brief A function to halt the CPU when a failure is detected during the very early boot process.
extern void miserableFail();

/// \brief The actual entry point of the microkernel image.
extern void start();

#endif //ZALEA_IMAGECONSTANTS_H
