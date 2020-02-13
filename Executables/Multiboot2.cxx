//===-- Multiboot2.c - The Multiboot Header ---------------------------------------------------------------*- C -*-===//
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

#include "Multiboot2.hxx"

extern "C" {
#include <ExecutableLibrary/ImageConstants.h>
}

/// This struct is the implementation of the Multiboot2 Header Tag.
MULTIBOOT_ATTR const struct multibootHeaderTag multibootHeader = {
        MULTIBOOT_HEADER_MAGIC,
        MULTIBOOT_HEADER_ARCHITECTURE,
        MULTIBOOT_HEADER_SIZEOF,
        static_cast<std::uint32_t>(MULTIBOOT_HEADER_CHECKSUM)
};

/// This struct is the implementation of the Multiboot2 Information Request Tag.
MULTIBOOT_ATTR const struct multibootInformationRequestTag multibootInformation = {  // NOLINT
        MULTIBOOT_TAG_INFORMATION_REQUEST,
        MULTIBOOT_REQUIRED(MULTIBOOT_TAG_INFORMATION_REQUEST_FLAGS),
        MULTIBOOT_TAG_INFORMATION_REQUEST_SIZEOF,
        {MULTIBOOT_TAG_TYPE_CMDLINE,
         MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME,
         MULTIBOOT_TAG_TYPE_BASIC_MEMINFO,
         MULTIBOOT_TAG_TYPE_BOOTDEV,
         MULTIBOOT_TAG_TYPE_MMAP,
         MULTIBOOT_TAG_TYPE_VBE,
         MULTIBOOT_TAG_TYPE_FRAMEBUFFER,
         MULTIBOOT_TAG_TYPE_APM,
         MULTIBOOT_TAG_TYPE_ACPI_OLD,
         MULTIBOOT_TAG_TYPE_ACPI_NEW,
         MULTIBOOT_TAG_TYPE_NETWORK,
         MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR,
         MULTIBOOT_TAG_TYPE_END}
};

/// This struct is the implementation of the Multiboot2 Address Synchronization Tag.
MULTIBOOT_ATTR const struct multibootAddressTag multibootAddress = {  // NOLINT
        MULTIBOOT_HEADER_TAG_ADDRESS,
        MULTIBOOT_REQUIRED(MULTIBOOT_HEADER_TAG_ADDRESS_FLAGS),
        MULTIBOOT_HEADER_TAG_ADDRESS_SIZEOF,
        reinterpret_cast<std::uint32_t>(&multibootHeader),  // NOLINT
        reinterpret_cast<std::uint32_t>(&imageStart),  // NOLINT
        reinterpret_cast<std::uint32_t>(&bssStart),  // NOLINT
        reinterpret_cast<std::uint32_t>(&imageEnd + 0x01)  // NOLINT
};

/// This struct is the implementation of the Multiboot2 Entry Point Address Tag.
MULTIBOOT_ATTR const struct multibootEntryAddressTag multibootEntryAddress = {  // NOLINT
        MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
        MULTIBOOT_REQUIRED(MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_FLAGS),
        MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_SIZEOF,
        reinterpret_cast<std::uint32_t>(&imageStart)  // NOLINT
};

/// This struct is the implementation of the Multiboot2 Header Tag.
MULTIBOOT_ATTR const struct multibootConsoleFlagsTag multibootConsoleFlags = {  // NOLINT
        MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS,
        MULTIBOOT_REQUIRED(MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_FLAGS),
        MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS_SIZEOF,
        MULTIBOOT_HEADER_CONSOLE_FLAG_REQUIRED(
                MULTIBOOT_HEADER_CONSOLE_FLAG_HAS_EGA(MULTIBOOT_HEADER_CONSOLE_FLAGS))
};

/// This struct is the implementation of the Multiboot2 Framebuffer Tag.
MULTIBOOT_ATTR const struct multibootFramebufferTag multibootFramebuffer = {  // NOLINT
        MULTIBOOT_HEADER_TAG_FRAMEBUFFER,
        MULTIBOOT_REQUIRED(MULTIBOOT_HEADER_TAG_FRAMEBUFFER_FLAGS),
        MULTIBOOT_HEADER_TAG_FRAMEBUFFER_SIZEOF,
        MULTIBOOT_HEADER_TAG_FRAMEBUFFER_WIDTH,
        MULTIBOOT_HEADER_TAG_FRAMEBUFFER_HEIGHT,
        MULTIBOOT_HEADER_TAG_FRAMEBUFFER_DEPTH
};

/// This struct is the implementation of the Multiboot2 Module Alignment Tag.
MULTIBOOT_ATTR const struct multibootModuleAlignmentTag multibootModuleAlignment = {  // NOLINT
        MULTIBOOT_HEADER_TAG_MODULE_ALIGN,
        MULTIBOOT_REQUIRED(MULTIBOOT_HEADER_TAG_MODULE_ALIGN_FLAGS),
        MULTIBOOT_HEADER_TAG_MODULE_ALIGN_SIZEOF,
};


