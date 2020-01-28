//===-- Multiboot2.h - The Multiboot Header  --------------------------------------------------------------*- C -*-===//
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
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the Multiboot 2 header which will be used by all architectures. Even tho it's only useful for x86
/// when it's being loaded by GRUB. This can also be useful when using the loader.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stdint.h>
#include <CompilerMagic.h>

#ifndef ZALEA_MULTIBOOT_2_H
#define ZALEA_MULTIBOOT_2_H

#define MULTIBOOT_ATTR ATTR_SECTION(".multiboot2") ATTR_ALIGNED(8)

#define MULTIBOOT_PROTECTED_MODE 0x00000000
#define MULTIBOOT_MIPS32 0x00000004

#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#ifdef KERNEL_x86
#define MULTIBOOT_HEADER_ARCHITECTURE MULTIBOOT_PROTECTED_MODE
#else
#define MULTIBOOT_HEADER_ARCHITECTURE 0xffffffff
#endif
#define MULIBOOT_HEADER_SIZEOF sizeof(struct multibootHeader)
#define MULTIBOOT_HEADER_CHECKSUM 0x100000000 - \
(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULIBOOT_HEADER_SIZEOF)

struct multibootHeader {
    uint32_t magic;
    uint32_t architecture;
    uint32_t size;
    uint32_t checksum;
};

#define MULTIBOOT_OPTIONAL_FLAG(x) x || (1UL << 0)
#define MULTIBOOT_REQUIRED_FLAG(x) x && ~(1UL << 0)

#define MULTIBOOT_TAG_INFORMATION 0x0001
#define MULIBOOT_TAG_INFORMATION_SIZEOF sizeof(_multibootInformationTag)

struct multibootInformationTag {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t mbi;
};

#endif //ZALEA_MULTIBOOT_2_H
