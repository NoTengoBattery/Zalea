//===-- Multiboot2.c - The Multiboot Header  --------------------------------------------------------------*- C -*-===//
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

#include <config.h>
#include "Multiboot2.h"

struct multibootHeader _multibootHeader MULTIBOOT_ATTR = {
        MULTIBOOT_HEADER_MAGIC,
        MULTIBOOT_HEADER_ARCHITECTURE,
        MULIBOOT_HEADER_SIZEOF,
        MULTIBOOT_HEADER_CHECKSUM
};

struct multibootInformationTag _multibootInformationTag MULTIBOOT_ATTR = {
        MULTIBOOT_TAG_INFORMATION_REQUEST,
        MULTIBOOT_REQUIRED_FLAG(MULTIBOOT_TAG_INFORMATION_REQUEST_FLAGS),
        MULTIBOOT_TAG_INFORMATION_REQUEST_SIZEOF,
        {MULTIBOOT_TAG_TYPE_CMDLINE,
         MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME,
         MULTIBOOT_TAG_TYPE_BASIC_MEMINFO,
         MULTIBOOT_TAG_TYPE_BOOTDEV,
         MULTIBOOT_TAG_TYPE_MMAP,
         MULTIBOOT_TAG_TYPE_FRAMEBUFFER,
         MULTIBOOT_TAG_TYPE_APM,
#ifdef KERNEL_x86_EFI
                MULTIBOOT_TAG_TYPE_EFI32,
#endif
#ifdef KERNEL_AMD64_EFI
                MULTIBOOT_TAG_TYPE_EFI64,
#endif
         MULTIBOOT_TAG_TYPE_SMBIOS,
         MULTIBOOT_TAG_TYPE_ACPI_OLD,
         MULTIBOOT_TAG_TYPE_ACPI_NEW,
         MULTIBOOT_TAG_TYPE_NETWORK,
#ifdef PLATFORM_EFI
        MULTIBOOT_TAG_TYPE_EFI_MMAP,
        MULTIBOOT_TAG_TYPE_EFI_BS,
#endif
#ifdef KERNEL_x86_EFI
                MULTIBOOT_TAG_TYPE_EFI32_IH,
#endif
#ifdef KERNEL_AMD64_EFI
                MULTIBOOT_TAG_TYPE_EFI64_IH,
#endif
         MULTIBOOT_TAG_TYPE_END}
};
