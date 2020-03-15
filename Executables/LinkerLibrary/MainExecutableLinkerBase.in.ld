/*===-- MainExecutableLinkerBase.in - The Base Linker Script -----------------------------------------------*- C -*-===*
 *
 * Copyright (c) 2020 Oever González
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 *  the License. You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 *  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 *  specific language governing permissions and limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * @formatter:off
 *
 *===--------------------------------------------------------------------------------------------------------------===*/
/***********************************************************************************************************************
 *
 * \file
 * This is the base of all linker scripts, so we include the whole file and use the macros inside the template to decide
 * which sections to use and how to fill them.
 *
 **********************************************************************************************************************/

#ifndef ZALEA_MAINEXECUTABLELINKERBASE_IN
#define ZALEA_MAINEXECUTABLELINKERBASE_IN

#include <LinkerLibrary/LinkerScriptDefinitions.in.ld>
#include <config.h>

#ifdef KERNEL_LINKER_GNU

PHDRS {
    start PT_LOAD FLAGS (PF_RX) ;
    multiboot PT_LOAD FLAGS (PF_R) ;
    code PT_LOAD FLAGS (PF_RX) ;
    data PT_LOAD FLAGS (PF_RW) ;
    rodata PT_LOAD FLAGS (PF_R) ;
    reserved PT_LOAD FLAGS (PF_RW) ;
    bss PT_LOAD FLAGS (PF_RW) ;
    stack PT_LOAD FLAGS (PF_RW) ;
}

SECTIONS {
    _SEGMENT_START("start-segment")
    .start ALIGN_COMMONPAGE : AT ( MACHINE_LOAD_ADDRESS /* MACHINE_LOAD_ADDRESS */ ) {
        PROVIDE ( imageStart = . );
        *(.start.header);
        *(.start);
    } :start
    ALIGNED_CPS_SEGMENT_START("multiboot-segment")
    .multiboot2 ALIGN_MINI : ALIGN_MINI {
        KEEP (*(.multiboot2));
        ASSERT (((. - imageStart) < 0x8000), "Multiboot header is out of bounds. Reduce the size of '.start' section.");
    } :multiboot
    ALIGNED_CPS_SEGMENT_START("text-segment")
    .text ALIGN_MINI : ALIGN_MINI {
        *(.text.unlikely);
        *(.text.hot);
        *(SORT(.text.sorted.*));
        *(.text .text.*);
    } :code
    ALIGNED_CPS_SEGMENT_START("data-segment")
    .data ALIGN_MINI : ALIGN_MINI {
        *(.data);
    } :data
    .eh_frame ALIGN_MINI : ALIGN_MINI ONLY_IF_RW {
        KEEP(*(.eh_frame));
        *(.eh_frame.*);
    }
    ALIGNED_CPS_SEGMENT_START("rodata-segment")
    .rodata ALIGN_MINI : ALIGN_MINI {
        *(.rodata);
        *(.rodata.*);
    } :rodata
    .note.gnu.build-id : {
        *(.note.gnu.build-id);
    }
#ifdef KERNEL_ARM
    .ARM.exidx ALIGN_MINI : ALIGN_MINI {
        PROVIDE_HIDDEN (exidxStart = .);
        *(.ARM.exidx*);
        *(.gnu.linkonce.armexidx.*);
        PROVIDE_HIDDEN (exidxEnd = .);
    }
#endif
    .eh_frame_hdr ALIGN_MINI : ALIGN_MINI {
        *(.eh_frame_hdr);
        *(.eh_frame_entry .eh_frame_entry.*);
    }
    .eh_frame ALIGN_MINI : ALIGN_MINI ONLY_IF_RO {
        KEEP (*(.eh_frame));
        *(.eh_frame.*);
    }
    /*
     * Give the system a small pool for guaranteed memory. This memory is guaranteed because it will be loaded along
     * with the image (making it bigger). These pages can be used by the very early boot process, so we can save some
     * precious data such as the bootloader information and some other computed data on the way. The PageKeeper driver
     * can use some of that memory... probably.
     */
    ALIGNED_CPS_SEGMENT_START("reserved-segment")
    .reserved ALIGN_MINI : ALIGN_MINI {
        reservedStart = .;
        *(.reserved);
        *(.reserved.*);
        /* 0x02: 8 KiB for architectures with pages of 4096 bytes */
        . += 0x02 * CONSTANT ( COMMONPAGESIZE );
        . = ALIGN_COMMONPAGE;
        reservedEnd = .;
    } :reserved
    /*
     * The stack and bss section, those sections are at the very end of the executable and extends towards the image
     * end.
     */
    ALIGNED_CPS_SEGMENT_START("bss-segment")
    .bss ALIGN_MINI (NOLOAD) : ALIGN_MINI {
        PROVIDE ( bssStart = . );
        *(.bss);
        *(COMMON);
        . = ALIGN_COMMONPAGE;
        PROVIDE ( bssEnd = . );
    } :bss
    /*
     * Adjust bss to the next page, this way, we can insert a RO page between BSS and the stack to detect accidental
     * stack overflows.
     */
#ifdef MACHINE_STACK_DOWNWARDS
     /*
      * Leave 1 page between bss and stack (it's a RO page which will rise exception upon stack overflow). This is
      * intended to prevent accidental overflows but will not help with malicious overflows.
      */
     . += MINI_ALIGN;
#endif
    ALIGNED_CPS_SEGMENT_START("stack-segment")
    .stack ALIGN_MINI (NOLOAD) : ALIGN_MINI {
#ifndef MACHINE_STACK_DOWNWARDS
        PROVIDE ( stackStart = . );
        PROVIDE ( stackLowermost = . );
#else
        PROVIDE ( stackEnd = . );
        PROVIDE ( stackLowermost = . );
#endif
        *(.stack);
        . = stackLowermost + MACHINE_STACK_SIZE;
#ifdef MACHINE_STACK_DOWNWARDS
        PROVIDE ( stackStart = . );
        PROVIDE ( imageEnd = . );
#else
        PROVIDE ( stackEnd = . );
        PROVIDE ( imageEnd = . );
#endif
    } :stack
#ifndef MACHINE_STACK_DOWNWARDS
    . += MINI_ALIGN;
    . = ALIGN_COMMONPAGE;
    stackGuardEnd = .;
#endif

    /*
     * Run 3 special assertions that will ensure the correct alignment of the entry point, load address and stack
     */
    ASSERT_VIRTUAL_ADDRESS
    ASSERT_LOAD_ADDRESS
    ASSERT_STACK_SIZE

    /*
     * Sections emitted by the compiler (they are stripped in the final binary)
     */
    .comment 0x00 : {
        *(.comment);
    }
#ifdef KERNEL_ARM
    .ARM.attributes 0x00 : {
        KEEP (*(.ARM.attributes));
    }
#endif

#else
#error "What linker are you using?"
#endif
}

#endif //ZALEA_MAINEXECUTABLELINKERBASE_IN
