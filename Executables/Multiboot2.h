//===-- Multiboot2.h - The Multiboot Header ---------------------------------------------------------------*- C -*-===//
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
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the Multiboot2 header which will be used by all architectures. Even tho it's only useful for x86
/// when it's being loaded by GRUB. This can also be useful when thinking about compressing the Main Executable.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_MULTIBOOT2_H
#define ZALEA_MULTIBOOT2_H

#include <BitwiseMacros.h>
#include <CompilerMagic.h>
#include <stdint.h>

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
#define MULIBOOT_HEADER_SIZEOF sizeof(struct multibootHeaderTag)
#define MULTIBOOT_HEADER_CHECKSUM (0x100000000 - \
(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULIBOOT_HEADER_SIZEOF))

struct multibootHeaderTag {
    const uint32_t magic;
    const uint32_t architecture;
    const uint32_t size;
    const uint32_t checksum;
};

/* These two macros define the "optional" bit flag, which is the bit 0 of the FLAGS field */
#define MULTIBOOT_OPTIONAL_FLAG(x) SET_NTH_BIT(x, 0)
#define MULTIBOOT_REQUIRED_FLAG(x) CLEAR_NTH_BIT(x, 0)

/* The following macros, enums and structs are the information request tag and responses, along with their magic types,
 * and their structure. */
#define MULTIBOOT_TAG_INFORMATION_REQUEST 0x0001
#define MULTIBOOT_TAG_INFORMATION_REQUEST_FLAGS 0x0000
#define MULTIBOOT_TAG_INFORMATION_REQUEST_SIZEOF sizeof(struct multibootInformationRequestTag)

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
#define MULTIBOOT_TAG_TYPE_EFI_32 0x0000000B
#define MULTIBOOT_TAG_TYPE_EFI_64 0x0000000C
#define MULTIBOOT_TAG_TYPE_SMBIOS 0x0000000D
#define MULTIBOOT_TAG_TYPE_ACPI_OLD 0x0000000E
#define MULTIBOOT_TAG_TYPE_ACPI_NEW 0x0000000F
#define MULTIBOOT_TAG_TYPE_NETWORK 0x00000010
#define MULTIBOOT_TAG_TYPE_EFI_MMAP 0x00000011
#define MULTIBOOT_TAG_TYPE_EFI_BS 0x00000012
#define MULTIBOOT_TAG_TYPE_EFI_32_IH 0x00000013
#define MULTIBOOT_TAG_TYPE_EFI_64_IH 0x00000014
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 0x00000015

enum {
    RequestNumber = 22
}; /* Because standard C does not support static flexible array initialization */

struct multibootInformationRequestTag {
    const uint16_t type;
    const uint16_t flags;
    const uint32_t size;
    const uint32_t requests[RequestNumber]; /* We use the (current) maximum amount of requests as the size of the array */
};

/* These macros and structs are the address tag, which is used to synchronize the physical address */
extern void *imageStart;
extern void *bssStart;
extern void *imageEnd;
#define MULTIBOOT_HEADER_TAG_ADDRESS 0x0002
#define MULTIBOOT_HEADER_TAG_ADDRESS_FLAGS 0x0000
#define  MULTIBOOT_HEADER_TAG_ADDRESS_SIZEOF sizeof(struct multibootAddressTag)
#define  MULTIBOOT_HEADER_TAG_ADDRESS_HEADER_ADDRESS &multibootHeader
#define  MULTIBOOT_HEADER_TAG_ADDRESS_LOAD_ADDRESS &imageStart
#define  MULTIBOOT_HEADER_TAG_ADDRESS_LOAD_END_ADDRESS &bssStart
#define  MULTIBOOT_HEADER_TAG_ADDRESS_BSS_END_ADDRESS &imageEnd

struct multibootAddressTag {
    const uint16_t type;
    const uint16_t flags;
    const uint32_t size;
    const uint32_t headerAddress;
    const uint32_t loadAddress;
    const uint32_t loadEndAddress;
    const uint32_t bssEndAddress;
};

/* These macros and structs are the entry tag, which tells the bootloader the physical address of the entry point */
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 0x0003
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_SIZEOF sizeof(struct multibootEntryAddressTag)
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_ENTRY_ADDRESS &imageStart

struct multibootEntryAddressTag {
    const uint16_t type;
    const uint16_t flags;
    const uint32_t size;
    const uint32_t entryAddress;
};

#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 0x0004
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_SIZEOF sizeof(struct multibootConsoleFlagsTag)
#define MULTIBOOT_HEADER_CONSOLE_FLAGS 0x0000
#define MULTIBOOT_HEADER_CONSOLE_FLAG_REQUIRED(x) SET_NTH_BIT(x, 0)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_OPTIONAL(x) CLEAR_NTH_BIT(x, 0)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_EGA(x) SET_NTH_BIT(x, 1)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_NO_EGA(x) CLEAR_NTH_BIT(x, 1)

struct multibootConsoleFlagsTag {
    const uint16_t type;
    const uint16_t flags;
    const uint32_t size;
    const uint32_t consoleFlags;
};

#endif //ZALEA_MULTIBOOT2_H
