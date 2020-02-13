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
/// time.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_IMAGECONSTANTS_H
#define ZALEA_IMAGECONSTANTS_H

/// This is a magic number which is expected to be present in the Multiboot2 i386 Machine State.
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

/// This is a pointer to the end of the bss section.
extern volatile void *bssEnd;
/// This is a pointer to the start of the bss section.
extern volatile void *bssStart;
/// This is a pointer to the end of the binary image.
extern volatile void *imageEnd;
/// This is a pointer to the start of the binary image.
extern volatile void *imageStart;

/// This is a pointer that will be the Multiboot2 as returned by the bootloader.
extern volatile void *multibootStructPointer;
/// This is a pointer to the end of the ARM vector table, which needs to be copied into memory.
extern volatile void *vecend;

/// A function to halt the CPU when a failure is detected during the very early boot process.
extern void miserableFail(void);

/// The actual entry point of the microkernel image.
extern void start(void);

#endif //ZALEA_IMAGECONSTANTS_H
