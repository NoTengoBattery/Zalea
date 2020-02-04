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

#ifndef ZALEA_MULTIBOOT2_HXX
#define ZALEA_MULTIBOOT2_HXX

extern "C" {
#include <CompilerMagic/CompilerMagic.h>
}

#include <CompilerMagic/BitwiseMacros.h>
#include <cstdint>

/* This is the attributes needed to assemble the Multiboot2 Header in the final image */
#define MULTIBOOT_ATTR ATTR_SECTION(".multiboot2") ATTR_USED
#define MULTIBOOT_ALIGNMENT 0x08

/* These two macros define the "optional" bit flag, which is the bit 0 of the FLAGS field */
#define MULTIBOOT_OPTIONAL(x) setNthBit(x, 0)
#define MULTIBOOT_REQUIRED(x) clearNthBit(x, 0)

/* This is the magic number that the OS will try to look when loaded by a Multiboot2 Bootloader */
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

/* These macros define the "architecture" to be requested to the bootloader */
#define MULTIBOOT_PROTECTED_MODE 0x00000000
#define MULTIBOOT_MIPS_32 0x00000004

/* These macros, enums and structs are the Multiboot2 Header, which is used to recognize a bootable image */
#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#ifdef KERNEL_x86
#define MULTIBOOT_HEADER_ARCHITECTURE MULTIBOOT_PROTECTED_MODE
#else
#define MULTIBOOT_HEADER_ARCHITECTURE 0xffffffff
#endif
#define MULTIBOOT_HEADER_SIZEOF sizeof(struct multibootHeaderTag)
#define MULTIBOOT_HEADER_CHECKSUM (0x100000000 - \
    (MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULTIBOOT_HEADER_SIZEOF))

struct alignas(MULTIBOOT_ALIGNMENT) multibootHeaderTag {
    const std::uint32_t magic;
    const std::uint32_t architecture;
    const std::uint32_t size;
    const std::uint32_t checksum;
};

/* The following macros and structs are the Information Request header tag, which request a bunch of information */
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
const unsigned int requestNumber = 22;

struct alignas(MULTIBOOT_ALIGNMENT) multibootInformationRequestTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t requests[requestNumber];  // NOLINT because we don't have a std::array here.
};

/* These macros and structs are the Address tag, which will synchronize the physical address with the bootloader */
#define MULTIBOOT_HEADER_TAG_ADDRESS 0x0002
#define MULTIBOOT_HEADER_TAG_ADDRESS_FLAGS 0x0000
#define  MULTIBOOT_HEADER_TAG_ADDRESS_SIZEOF sizeof(struct multibootAddressTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t headerAddress;
    const std::uint32_t loadAddress;
    const std::uint32_t loadEndAddress;
    const std::uint32_t bssEndAddress;
};

/* These macros and structs are the Entry Address tag, which will tell the bootloader where to jump after loading */
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 0x0003
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_SIZEOF sizeof(struct multibootEntryAddressTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootEntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the Console Flags tag, which will tell the bootloader where to jump after loading */
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 0x0004
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_SIZEOF sizeof(struct multibootConsoleFlagsTag)
#define MULTIBOOT_HEADER_CONSOLE_FLAGS 0x0000
#define MULTIBOOT_HEADER_CONSOLE_FLAG_REQUIRED(x) setNthBit(x, 0)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_OPTIONAL(x) clearNthBit(x, 0)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_EGA(x) setNthBit(x, 1)
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_NO_EGA(x) clearNthBit(x, 1)

struct alignas(MULTIBOOT_ALIGNMENT) multibootConsoleFlagsTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t consoleFlags;
};

/* These macros and structs are the Framebuffer tag, which will tell the bootloader to initialize a framebuffer */
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 0x0005
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZEOF sizeof(struct multibootFramebufferTag)
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_WIDTH 0x0000
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_HEIGHT 0x0000
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_DEPTH 0x0000

struct alignas(MULTIBOOT_ALIGNMENT) multibootFramebufferTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t width;
    const std::uint32_t height;
    const std::uint32_t depth;
};

/* These macros and structs are the Module Alignment tag, which will tell the bootloader to page-align the modules */
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN 0x0006
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN_SIZEOF sizeof(struct multibootModuleAlignmentTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootModuleAlignmentTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
};

/* These macros and structs are the EFI Boot Services tag, which will tell the bootloader to boot as in EFI mode */
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES 0x0007
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES_SIZEOF sizeof(struct multibootEfiBootServicesTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootEfiBootServicesTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
};

/* These macros and structs are the EFI32 Entry Address tag, which will tell the bootloader to jump into EFI32 code */
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32 0x0008
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32_SIZEOF sizeof(struct multibootEfi32EntryAddressTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootEfi32EntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the EFI64 Entry Address tag, which will tell the bootloader to jump into EFI64 code */
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64 0x0009
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64_SIZEOF sizeof(struct multibootEfi64EntryAddressTag)

struct alignas(MULTIBOOT_ALIGNMENT) multibootEfi64EntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the Relocatable tag, which will tell the bootloader that this image is relocatable */
#define MULTIBOOT_HEADER_TAG_RELOCATABLE 0x000A
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_FLAGS 0x0000
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_SIZEOF sizeof(struct multibootRelocatableTag)
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_NONE 0x0000
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_LOWEST 0x0001
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_HIGHEST 0x0002

struct alignas(MULTIBOOT_ALIGNMENT) multibootRelocatableTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t minimumAddress;
    const std::uint32_t maximumAddress;
    const std::uint32_t alignment;
    const std::uint32_t preference;
};

#endif //ZALEA_MULTIBOOT2_HXX
