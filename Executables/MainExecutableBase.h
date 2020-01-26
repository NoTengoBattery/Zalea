//===-- MainExecutableBase.h - The Base Linker Script  ----------------------------------------------------*- C -*-===//
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
/// This is the base of all linker scripts, so we include the whole file and use the macros inside the template to
/// decide which sections to use and how to fill them.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_MAINEXECUTABLEBASE_H
#define ZALEA_MAINEXECUTABLEBASE_H
#include <config.h>
#include "LinkerScriptDefinitions.h"

#ifdef KERNEL_LINKER_GNU
    _ASSERT_VIRTUAL_ADDRESS
    _ASSERT_LOAD_ADDRESS

    _SEGMENTSTART("text-segment")
    .start : AT(MACHINE_LOAD_ADDRESS) {
        _start_start = .;
        *(.start);
        _start_end = .;
    }
    .text ALIGN(MINI_ALIGN) : ALIGN(MINI_ALIGN) {
        _text_start = .;
        *(.text.unlikely);
        *(.text.hot);
        *(SORT(.text.sorted.*));
        *(.text .text.*);
        _text_end = .;
    }
    _ALIGNED_CPS_SEGMENTSTART("rodata-segment")
    .rodata ALIGN(MINI_ALIGN) : ALIGN(MINI_ALIGN) {
        _rodata_start = .;
        *(.rodata);
        _rodata_end = .;
    }
    _ALIGNED_CPS_SEGMENTSTART("data-segment")
    .data ALIGN(MINI_ALIGN) : ALIGN(MINI_ALIGN) {
        _data_start = .;
        *(.data);
        _data_end = .;
    }
    _ALIGNED_CPS_SEGMENTSTART("bss-segment")
    .bss ALIGN(MINI_ALIGN) : ALIGN(MINI_ALIGN) {
        _bss_start = .;
        *(.bss);
        *(COMMON);
        _bss_end = .;
    }


    .stack MACHINE_VIRTUAL_ADDRESS : AT(MACHINE_LOAD_ADDRESS) {
        _stack_start = .;
        *(.stack);
        _stack_end = .;
    }


    .comment 0 : {
        *(.comment);
    }
#ifdef KERNEL_ARM
    .ARM.attributes 0 : {
        KEEP (*(.ARM.attributes));
    }
#endif
#endif
#endif //ZALEA_MAINEXECUTABLEBASE_H
