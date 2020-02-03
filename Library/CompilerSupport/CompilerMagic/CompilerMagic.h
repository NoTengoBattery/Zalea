//===-- CompilerMagic.h - Support and Alleviate Differences Between Compilers -----------------------------*- C -*-===//
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
/// This file can be imported inside any other target, and used inside any source file where compiler-specific
/// directives are needed. This file will attempt to mask the differences between the supported compilers.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_COMPILERMAGIC_H
#define ZALEA_COMPILERMAGIC_H

#include <config.h>

#ifdef KERNEL_COMPILER_GNU

#ifdef __ASSEMBLER__
#error "This file should not be included in Assembler."
#else

#define ATTR_ALIGNED(x) __attribute__ ((aligned (x)))
#define ATTR_FASTCALL __attribute__ ((fastcall))
#define ATTR_NAKED __attribute__ ((naked))
#define ATTR_NORETURN __attribute__ ((noreturn))
#define ATTR_REGPARM(x) __attribute__ ((regparm (x)))
#define ATTR_SECTION(x) __attribute__ ((section (x)))
#define ATTR_USED __attribute__ ((used))
#define BUILTIN_UNREACHABLE __builtin_unreachable()
#define REORDERING_BARRIER __asm__ __volatile__("":::"memory")

#endif

#else
#error "What compiler are you using?"
#endif

#endif //ZALEA_COMPILERMAGIC_H
