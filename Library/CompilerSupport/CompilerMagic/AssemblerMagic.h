//===-- AssemblerMagic.h - Macros for Assembly Source Files ---------------------------------------------*- ASM -*-===//
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
// @formatter:off
//
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// All of the macros in here should be used inside the ASM files, they define the correct prologues and epilogues for
/// all assemblers for all architectures.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_ASSEMBLERMAGIC_H
#define ZALEA_ASSEMBLERMAGIC_H

#include <config.h>

#ifdef KERNEL_BINUTILS_GNU

#ifndef __ASSEMBLER__
#error "You should include this file exclusively inside preprocessed assembler files."
#endif

#ifndef ASM_NL
#define ASM_NL ;
#endif

    .file __FILENAME__

#ifdef KERNEL_x86 // All of the x86 specific macros for the GNU assembler

    .intel_syntax noprefix // All x86 code should be in the x86 Intel Syntax without prefix

#define NAKED_GLOBAL_FUNCTION(s, n) \
    .text ASM_NL \
    .section s, "ax", @progbits ASM_NL \
    .align 0x10 ASM_NL \
    .global n ASM_NL \
    .type n, @function ASM_NL \
    n:

#define GENERIC_EPILOG(n) \
    .size n, . - n ASM_NL

#define NAKED_GLOBAL_FUNCTION_EPILOG(n) GENERIC_EPILOG(n)

#define GLOBAL_DATA_2(n, s, f, b) \
    .global n ASM_NL \
    .section s, f, b ASM_NL \
    n:

#define GLOBAL_DATA(n) GLOBAL_DATA_2(n, .data, "aw", @progbits)
#define GLOBAL_RODATA(n) GLOBAL_DATA_2(n, .rodata, "a", @progbits)

#define GLOBAL_DATA_2_EPILOG(n) \
    .type n, @object ASM_NL \
    .size n, . - n

#define GLOBAL_DATA_EPILOG(n) GLOBAL_DATA_2_EPILOG(n)
#define GLOBAL_RODATA_EPILOG(n) GLOBAL_DATA_2_EPILOG(n)

#define GLOBAL_BSS_DATA(n, s) GLOBAL_DATA_2(n, .bss, "aw", @nobits) ASM_NL \
    .skip s, 0x00 ASM_NL \
    GLOBAL_DATA_2_EPILOG(n)
#define GLOBAL_STACK_DATA(n, s) GLOBAL_DATA_2(n, .stack, "aw", @nobits) ASM_NL \
    .skip s, 0x00 ASM_NL \
    GLOBAL_DATA_2_EPILOG(n)

    .ident "x86 AssemblerMagic for GNU Assemblers"

#endif

#else
#error "What assembler are you using?"
#endif

#endif //ZALEA_ASSEMBLERMAGIC_H
