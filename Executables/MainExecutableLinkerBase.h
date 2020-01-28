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

#ifndef ZALEA_MAINEXECUTABLELINKERBASE_H
#define ZALEA_MAINEXECUTABLELINKERBASE_H

#include <config.h>
#include "LinkerScriptDefinitions.h"

#ifdef KERNEL_LINKER_GNU

    _SEGMENTSTART("text-segment")
    .start _ALIGN_COMMONPAGE : AT(MACHINE_LOAD_ADDRESS) _ALIGN_COMMONPAGE {
        *(.start);
    }
    .multiboot2 _ALIGN_MINI : _ALIGN_MINI {
        KEEP (*(.multiboot2));
    }
    .text _ALIGN_MINI : _ALIGN_MINI {
        *(.text.unlikely);
        *(.text.hot);
        *(SORT(.text.sorted.*));
        *(.text .text.*);
    }
    _ALIGNED_CPS_SEGMENTSTART("data-segment")
    .data _ALIGN_MINI : _ALIGN_MINI {
        *(.data);
    }
    .eh_frame _ALIGN_MINI : _ALIGN_MINI ONLY_IF_RW {
        KEEP(*(.eh_frame));
        *(.eh_frame.*);
    }
    _ALIGNED_CPS_SEGMENTSTART("bss-segment")
    .bss _ALIGN_MINI : _ALIGN_MINI {
        *(.bss);
        *(COMMON);
    }
    _ALIGNED_CPS_SEGMENTSTART("rodata-segment")
    .rodata _ALIGN_MINI : _ALIGN_MINI {
        *(.rodata);
    }
    .eh_frame_hdr _ALIGN_MINI : _ALIGN_MINI {
        *(.eh_frame_hdr);
        *(.eh_frame_entry .eh_frame_entry.*);
    }
    .eh_frame _ALIGN_MINI : _ALIGN_MINI ONLY_IF_RO {
        KEEP (*(.eh_frame));
        *(.eh_frame.*);
    }


    // The stack section, this section is the start of the executable and reaches until the entry point
    _ALIGNED_CPS_SEGMENTSTART("stack-segment")
    .stack _ALIGN_COMMONPAGE : _ALIGN_COMMONPAGE {
#ifndef MACHINE_STACK_DOWNWARDS
        _stack_start = .; _stack_lowermost = .;
#else
        _stack_end = .; _stack_lowermost = .;
#endif
        *(.stack);
        . = _stack_lowermost + MACHINE_STACK_SIZE;
#ifdef MACHINE_STACK_DOWNWARDS
        _stack_start = .; _image_end = .;
#else
        _stack_end = .; _image_end = .;
#endif
    }


    // Sections emitted by the compiler (they are stripped in the final binary)
    .comment 0 : {
        *(.comment);
    }
#ifdef KERNEL_ARM
    .ARM.attributes 0 : {
        KEEP (*(.ARM.attributes));
    }
#endif

    // Run 4 special assertions that will ensure the correct alignment of the entry point, load address and stack
    _ASSERT_VIRTUAL_ADDRESS
    _ASSERT_LOAD_ADDRESS
    _ASSERT_STACK_SIZE

#else
#error "What linker are you using?"
#endif

#endif //ZALEA_MAINEXECUTABLELINKERBASE_H