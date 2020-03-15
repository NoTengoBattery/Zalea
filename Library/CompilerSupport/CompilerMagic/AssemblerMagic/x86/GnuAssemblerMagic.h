//===-- GnuAssemblerMagic.h - Macros for Assembly Source Files ------------------------------------------*- ASM -*-===//
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

#ifndef ZALEA_GNUASSEMBLERMAGIC_H
#define ZALEA_GNUASSEMBLERMAGIC_H

#ifdef __MAGIC_INCLUDED__

  .intel_syntax noprefix // All x86 code should be in the x86 Intel Syntax without prefix

#define NAKED_GLOBAL_FUNCTION(s, n) \
  .text ASM_NL \
  .section s, "ax", @progbits ASM_NL \
  .align 0x10 ASM_NL \
  .global n ASM_NL \
  .type n, @function ASM_NL \
  n:

#define GENERIC_FUNCTION_EPILOG(n) \
  .size n, . - n

#define NAKED_GLOBAL_FUNCTION_EPILOG(n) GENERIC_FUNCTION_EPILOG(n)

#define GLOBAL_DATA_2(n, s, f, b) \
  .section s, f, b ASM_NL \
  .align 0x04 ASM_NL \
  .## n ##:

#define GLOBAL_DATA(n) GLOBAL_DATA_2(n, .data, "aw", @progbits)
#define GLOBAL_RODATA(n) GLOBAL_DATA_2(n, .rodata, "a", @progbits)
#define GLOBAL_RESERVED(n) GLOBAL_DATA_2(n, .reserved, "a", @progbits)

#define GLOBAL_DATA_2_EPILOG(n) \
  .align 0x04 ASM_NL \
  .type n, @object ASM_NL \
  .global n ASM_NL \
  n: ASM_NL \
  .long . ## n ASM_NL \
  .size n, . - n

#define GLOBAL_DATA_EPILOG(n) GLOBAL_DATA_2_EPILOG(n)
#define GLOBAL_RODATA_EPILOG(n) GLOBAL_DATA_2_EPILOG(n)
#define GLOBAL_RESERVED_EPILOG(n) GLOBAL_DATA_2_EPILOG(n)

#define GLOBAL_DATA_3(n, s, f, b) \
  .section s, f, b ASM_NL \
  .global n ASM_NL \
  n:

#define GLOBAL_DATA_3_EPILOG(n) \
  .type n, @object ASM_NL \
  .size n, . - n

#define GLOBAL_BSS_DATA(n, s) GLOBAL_DATA_3(n, .bss, "aw", @nobits) ASM_NL \
  .skip (s * 0x04), 0x00 ASM_NL \
  GLOBAL_DATA_3_EPILOG(n)

#define GLOBAL_STACK_DATA(n, s) GLOBAL_DATA_3(n, .stack, "aw", @nobits) ASM_NL \
  .skip (s * 0x04), 0x00 ASM_NL \
  GLOBAL_DATA_3_EPILOG(n)

#define LOCAL_DATA_2(n, s, f, b) \
  .section s, f, b ASM_NL \
  n:

#define LOCAL_DATA_EPILOG(n) GLOBAL_DATA_3_EPILOG(n)

#define LOCAL_BSS_DATA(n, s) LOCAL_DATA_2(n, .bss, "aw", @nobits) ASM_NL \
  .skip (s * 0x04), 0x00 ASM_NL \
  LOCAL_DATA_EPILOG(n)

  .ident "x86 AssemblerMagic for GNU Assemblers"

#else
#error "You should not include this file directly. Use the root header instead."
#endif

#endif //ZALEA_GNUASSEMBLERMAGIC_H
