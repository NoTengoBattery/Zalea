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
//  @formatter:off
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the Multiboot 2 header which will be used by all architectures. Even tho it's only useful for x86
/// when it's being loaded by GRUB. This can also be useful when using the loader.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <stdint.h>
#define MULTIBOOT_ATTRIBUTES __attribute__ ((section (".multiboot2")))
#define MULTIBOOT_MAGIC 0xE85250D6

struct MultiBootHeader {
    uint32_t magic;
};
