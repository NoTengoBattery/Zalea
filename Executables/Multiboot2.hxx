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

#include <CompilerMagic/BitwiseMacros.h>
#include <CompilerMagic/CompilerMagic.h>
#include <cstdint>

/* This is the attributes needed to assemble the Multiboot2 Header in the final image */
/// Attributes needed to correctly compile and place the multiboot headers.
#define MULTIBOOT_ATTR ATTR_SECTION(".multiboot2") ATTR_USED
/// The alignment (in bytes) required by the Multiboot2 specification.
#define MULTIBOOT_ALIGNMENT 0x08

/* These two macros define the "optional" bit flag, which is the bit 0 of the FLAGS field */
/// A macro that will mark a Multiboot2 flag as optional.
#define MULTIBOOT_OPTIONAL(x) setNthBit(x, 0)
/// A macro that will mark a Multiboot2 flag as required.
#define MULTIBOOT_REQUIRED(x) clearNthBit(x, 0)

/* This is the magic number that the OS will try to look when loaded by a Multiboot2 Bootloader */
/// A magic number that the Multiboot2 bootloader will hand to the OS in order to identify itself.
#define MULTIBOOT_2_BOOTLOADER_MAGIC 0x36D76289

/* These macros define the "architecture" to be requested to the bootloader */
/// A magic number that ask the bootloader to load the OS image in the x86 Protected Mode.
#define MULTIBOOT_PROTECTED_MODE 0x00000000
/// A magic number that ask the bootloader to load the OS image in the MIPS32 Mode.
#define MULTIBOOT_MIPS_32 0x00000004

/* These macros, enums and structs are the Multiboot2 Header, which is used to recognize a bootable image */
/// A magic number that the bootloader looks into the image to check if it's a valid Multiboot2 OS.
#define MULTIBOOT_HEADER_MAGIC 0xE85250D6
#ifdef KERNEL_x86
/// The selected Multiboot2 architecture.
#define MULTIBOOT_HEADER_ARCHITECTURE MULTIBOOT_PROTECTED_MODE
#else
/// The selected Multiboot2 architecture.
#define MULTIBOOT_HEADER_ARCHITECTURE 0xffffffff
#endif
/// Size of the Multiboot2 Header tag.
#define MULTIBOOT_HEADER_SIZEOF sizeof(struct multibootHeaderTag)
/// Checksum for the Multiboot2 Header tag that the bootloader will verify to load the OS image.
#define MULTIBOOT_HEADER_CHECKSUM (0x100000000 - \
    (MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_ARCHITECTURE + MULTIBOOT_HEADER_SIZEOF))

/// The Multiboot2 Header Tag. The bootloader searches for this tag and verifies it's integrity.
struct alignas(MULTIBOOT_ALIGNMENT) multibootHeaderTag {
    const std::uint32_t magic;
    const std::uint32_t architecture;
    const std::uint32_t size;
    const std::uint32_t checksum;
};

/* The following macros and structs are the Information Request header tag, which request a bunch of information */
/// The Information Request tag ID.
#define MULTIBOOT_TAG_INFORMATION_REQUEST 0x0001
/// The Information Request tag initial flags.
#define MULTIBOOT_TAG_INFORMATION_REQUEST_FLAGS 0x0000
/// The size of the Information Request tag.
#define MULTIBOOT_TAG_INFORMATION_REQUEST_SIZEOF sizeof(struct multibootInformationRequestTag)

/// The Information Request tag for the end of the tag list.
#define MULTIBOOT_TAG_TYPE_END 0x00000000
/// The Information Request tag for asking a command line.
#define MULTIBOOT_TAG_TYPE_CMDLINE 0x00000001
/// The Information Request tag for asking the bootloader name.
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 0x00000002
/// The Information Request tag for asking the bootloader to load modules to the OS.
#define MULTIBOOT_TAG_TYPE_MODULE 0x00000003
/// The Information Request tag for asking a basic memory information.
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 0x00000004
/// The Information Request tag for asking for the device from where the OS was loaded.
#define MULTIBOOT_TAG_TYPE_BOOTDEV 0x00000005
/// The Information Request tag for asking a memory map.
#define MULTIBOOT_TAG_TYPE_MMAP 0x00000006
/// The Information Request tag for asking VBE graphics.
#define MULTIBOOT_TAG_TYPE_VBE 0x00000007
/// The Information Request tag for asking a framebuffer.
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 0x00000008
/// The Information Request tag for asking the ELF sections of the loaded image.
#define MULTIBOOT_TAG_TYPE_ELF_SECTIONS 0x00000009
/// The Information Request tag for asking the APM information.
#define MULTIBOOT_TAG_TYPE_APM 0x0000000A
/// The Information Request tag for asking to boot as a EFI32 application.
#define MULTIBOOT_TAG_TYPE_EFI_32 0x0000000B
/// The Information Request tag for asking to boot as a EFI64 application.
#define MULTIBOOT_TAG_TYPE_EFI_64 0x0000000C
/// The Information Request tag for asking the SMBIOS information (not supported by GRUB).
#define MULTIBOOT_TAG_TYPE_SMBIOS 0x0000000D
/// The Information Request tag for asking for the old ACPI mode.
#define MULTIBOOT_TAG_TYPE_ACPI_OLD 0x0000000E
/// The Information Request tag for asking for the new ACPI mode.
#define MULTIBOOT_TAG_TYPE_ACPI_NEW 0x0000000F
/// The Information Request tag for asking for information about network devices.
#define MULTIBOOT_TAG_TYPE_NETWORK 0x00000010
/// The Information Request tag for asking for a EFI MMAP.
#define MULTIBOOT_TAG_TYPE_EFI_MMAP 0x00000011
/// The Information Request tag for asking a EFI BS.
#define MULTIBOOT_TAG_TYPE_EFI_BS 0x00000012
/// The Information Request tag for asking a EFI32 IH.
#define MULTIBOOT_TAG_TYPE_EFI_32_IH 0x00000013
/// The Information Request tag for asking a EFI63 IH.
#define MULTIBOOT_TAG_TYPE_EFI_64_IH 0x00000014
/// The Information Request tag for asking the base address of the loaded OS.
#define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 0x00000015
/// The total number of Information Request tags.
const unsigned int requestNumber = 22;

/// The Multiboot2 Information Request Tag. The bootloader will try to hand over this information to the OS.
struct alignas(MULTIBOOT_ALIGNMENT) multibootInformationRequestTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t requests[requestNumber];
};

/* These macros and structs are the Address tag, which will synchronize the physical address with the bootloader */
/// The Address Header tag ID.
#define MULTIBOOT_HEADER_TAG_ADDRESS 0x0002
/// The Address Header initial flags.
#define MULTIBOOT_HEADER_TAG_ADDRESS_FLAGS 0x0000
/// The size of the Address Header tag.
#define  MULTIBOOT_HEADER_TAG_ADDRESS_SIZEOF sizeof(struct multibootAddressTag)

/// The Address Header Tag. This tag will synchronize the bootloader and the OS addresses.
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
/// The Entry Address Header tag ID.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS 0x0003
/// The Entry Address Header initial flags.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_FLAGS 0x0000
/// The size of the Entry Address Header tag.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_SIZEOF sizeof(struct multibootEntryAddressTag)

/// The Entry Address Header Tag. This tag will tell the bootloader where to jump when the OS image is loaded.
struct alignas(MULTIBOOT_ALIGNMENT) multibootEntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the Console Flags tag, which will tell the bootloader where to jump after loading */
/// The Console Header tag ID.
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 0x0004
/// The Console Header initial flags.
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_FLAGS 0x0000
/// The size of the Console Header tag.
#define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_SIZEOF sizeof(struct multibootConsoleFlagsTag)
/// The Console tag flags.
#define MULTIBOOT_HEADER_CONSOLE_FLAGS 0x0000
/// This macro will set the console as required.
#define MULTIBOOT_HEADER_CONSOLE_FLAG_REQUIRED(x) setNthBit(x, 0)
/// This macro will set the console as optional.
#define MULTIBOOT_HEADER_CONSOLE_FLAG_OPTIONAL(x) clearNthBit(x, 0)
/// This macro will say the bootloader that there is support for the EGA console.
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_EGA(x) setNthBit(x, 1)
/// This macro will say the bootloader that there is no support for the EGA console.
#define MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_NO_EGA(x) clearNthBit(x, 1)

/// The Console Header Tag. This tag will ask the bootloader a console, and will tell the bootloader it's requirements.
struct alignas(MULTIBOOT_ALIGNMENT) multibootConsoleFlagsTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t consoleFlags;
};

/* These macros and structs are the Framebuffer tag, which will tell the bootloader to initialize a framebuffer */
/// The Framebuffer Header tag ID.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER 0x0005
/// The Framebuffer Header initial flags.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_FLAGS 0x0000
/// The size of the Framebuffer Header.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZEOF sizeof(struct multibootFramebufferTag)
/// The width of te Framebuffer in pixels. A value of 0 means no preference.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_WIDTH 0x0000
/// The height of te Framebuffer in pixels. A value of 0 means no preference.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_HEIGHT 0x0000
/// The bit depth of te Framebuffer. A value of 0 means no preference.
#define MULTIBOOT_HEADER_TAG_FRAMEBUFFER_DEPTH 0x0000

/// The Framebuffer Header Tag. This tag asks the bootloader a framebuffer and set it's properties.
struct alignas(MULTIBOOT_ALIGNMENT) multibootFramebufferTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t width;
    const std::uint32_t height;
    const std::uint32_t depth;
};

/* These macros and structs are the Module Alignment tag, which will tell the bootloader to page-align the modules */
/// The Module Alignment Header tag ID.
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN 0x0006
/// The Module Alignment Header initial flags.
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN_FLAGS 0x0000
/// The size of the Module Alignment Header.
#define MULTIBOOT_HEADER_TAG_MODULE_ALIGN_SIZEOF sizeof(struct multibootModuleAlignmentTag)

/// The Module Alignment Header Tag. This tag will tell the bootloader to align modules to the page boundary.
struct alignas(MULTIBOOT_ALIGNMENT) multibootModuleAlignmentTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
};

/* These macros and structs are the EFI Boot Services tag, which will tell the bootloader to boot as in EFI mode */
/// The EFI Boot Services Header tag ID.
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES 0x0007
/// The EFI Boot Services Header initial flags.
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES_FLAGS 0x0000
/// The size of the EFI Boot Services Header.
#define MULTIBOOT_HEADER_TAG_EFI_BOOT_SERVICES_SIZEOF sizeof(struct multibootEfiBootServicesTag)

/// \brief The EFI Boot Services Header Tag. This tag tells the bootloader that the OS should be loaded without
/// terminating the EFI boot services.
struct alignas(MULTIBOOT_ALIGNMENT) multibootEfiBootServicesTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
};

/* These macros and structs are the EFI32 Entry Address tag, which will tell the bootloader to jump into EFI32 code */
/// The EFI32 Entry Address Header tag ID.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32 0x0008
/// The EFI32 Entry Address Header initial flags.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32_FLAGS 0x0000
/// The size of the EFI32 Entry Address Header.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_32_SIZEOF sizeof(struct multibootEfi32EntryAddressTag)

/// The EFI32 Entry Address Header Tag. This tag tells the bootloader where to begin executing EFI32 code.
struct alignas(MULTIBOOT_ALIGNMENT) multibootEfi32EntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the EFI64 Entry Address tag, which will tell the bootloader to jump into EFI64 code */
/// The EFI64 Entry Address Header tag ID.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64 0x0009
/// The EFI64 Entry Address Header initial flags.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64_FLAGS 0x0000
/// The size of the EFI64 Entry Address Header.
#define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI_64_SIZEOF sizeof(struct multibootEfi64EntryAddressTag)

/// The EFI64 Entry Address Header Tag. This tag tells the bootloader where to begin executing EFI64 code.
struct alignas(MULTIBOOT_ALIGNMENT) multibootEfi64EntryAddressTag {
    const std::uint16_t type;
    const std::uint16_t flags;
    const std::uint32_t size;
    const std::uint32_t entryAddress;
};

/* These macros and structs are the Relocatable tag, which will tell the bootloader that this image is relocatable */
/// The Relocatable tag ID.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE 0x000A
/// The Relocatable initial flags.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_FLAGS 0x0000
/// The size of the Relocatable tag.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_SIZEOF sizeof(struct multibootRelocatableTag)
/// The Relocatable preference: none.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_NONE 0x0000
/// The Relocatable preference: lowest possible memory address.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_LOWEST 0x0001
/// The Relocatable preference: highest possible memory address.
#define MULTIBOOT_HEADER_TAG_RELOCATABLE_PREFERENCE_HIGHEST 0x0002

/// \brief The Relocatable Header Tag. This tag tells the bootloader that the image is relocatable, and tells it's
/// preferences to where it may be located.
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
