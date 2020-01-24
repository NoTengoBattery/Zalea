//===-- LLVM.MainExecutable.in.ld - LLVM Linker Script --------------------------------------------------*- C++ -*-===//
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
/// This file is a LLVM linker script which will be preprocessed and then used for the linker to generate the Main
/// Executable, which is the monolithic part of this project.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <config.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x8000));
    . = SEGMENT_START("text-segment", 0x8000);

    .start                                                  :                   { *(.start) }
}
