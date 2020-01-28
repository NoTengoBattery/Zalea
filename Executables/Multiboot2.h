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
#include <BitwiseMacros.h>

#ifndef ZALEA_MULTIBOOT_2_H
#define ZALEA_MULTIBOOT_2_H

/* This is the C attributes needed to build the table in the final image */
#define MULTIBOOT_ATTR ATTR_SECTION(".multiboot2") ATTR_ALIGNED(8) ATTR_USED

/* This is the magic number that the OS will try to look when loaded by a Multiboot2 bootloader */
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

/* These macros define the "architecture" to be requested to the bootloader */
#define MULTIBOOT_PROTECTED_MODE 0x00000000
#define MULTIBOOT_MIPS_32 0x00000004

/* These macros, enums and structs are the basic Multiboot2 header, which is used to recognize a bootable image */
#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#ifdef KERNEL_x86
#define MULTIBOOT_HEADER_ARCHITECTURE MULTIBOOT_PROTECTED_MODE
#else
#define MULTIBOOT_HEADER_ARCHITECTURE 0xffffffff
#endif
#define MULIBOOT_HEADER_SIZEOF sizeof(struct multibootHeader)
#define MULTIBOOT_HEADER_CHECKSUM (0x100000000 - \
(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULIBOOT_HEADER_SIZEOF))

struct multibootHeader {
    uint32_t magic;
    uint32_t architecture;
    uint32_t size;
    uint32_t checksum;
};

/* These two macros define the "optional" bit flag, which is the bit 0 of the FLAGS field */
#define MULTIBOOT_OPTIONAL_FLAG(x) SET_UNSIGNED_NBIT(x, 0)
#define MULTIBOOT_REQUIRED_FLAG(x) CLEAR_UNSIGNED_NBIT(x, 0)

/* The following macros, enums and structs are the information request headers and responses, along with their magic
 * types, and their structure. */
#define MULTIBOOT_TAG_INFORMATION_REQUEST 0x0001
#define MULTIBOOT_TAG_INFORMATION_REQUEST_FLAGS 0x0000
#define MULTIBOOT_TAG_INFORMATION_REQUEST_SIZEOF sizeof(_multibootInformationTag)

#define MULTIBOOT_TAG_TYPE_END 0x00000000
#define MULTIBOOT_TAG_TYPE_CMDLINE 0x00000001
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 0x00000002
#define MULTIBOOT_TAG_TYPE_MODULE 0x00000003
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 0x00000004
#define MULTIBOOT_TAG_TYPE_BOOTDEV 0x00000005
#define MULTIBOOT_TAG_TYPE_MMAP 0x00000006
#define MULTIBOOT_TAG_TYPE_VBE 0x00000007
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 0x00000008
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS 0x00000009
#define MULTIBOOT_TAG_TYPE_APM 0x0000000A
#define MULTIBOOT_TAG_TYPE_EFI32 0x0000000B
#define MULTIBOOT_TAG_TYPE_EFI64 0x0000000C
#define MULTIBOOT_TAG_TYPE_SMBIOS 0x0000000D
#define MULTIBOOT_TAG_TYPE_ACPI_OLD 0x0000000E
#define MULTIBOOT_TAG_TYPE_ACPI_NEW 0x0000000F
#define MULTIBOOT_TAG_TYPE_NETWORK 0x00000010
#define MULTIBOOT_TAG_TYPE_EFI_MMAP 0x00000011
#define MULTIBOOT_TAG_TYPE_EFI_BS 0x00000012
#define MULTIBOOT_TAG_TYPE_EFI32_IH 0x00000013
#define MULTIBOOT_TAG_TYPE_EFI64_IH 0x00000014
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 0x00000015

enum {
    RequestNumber = 22
}; /* Because standard C does not support static flexible array initialization */

struct multibootInformationTag {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
    uint32_t requests[RequestNumber]; /* We use the (current) maximum amount of requests as the size of the array */
};

#endif //ZALEA_MULTIBOOT_2_H
