//===-- CompilerMagic.h - Support and Alleviate Differences Between Compilers ---------------------------*- C++ -*-===//
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
#error "This file should not be included in Assembler source files."
#else

/// \brief Set the `aligned` attribute.
///
/// \param x the alignment in bytes.
#define ATTR_ALIGNED(x) __attribute__ ((aligned (x)))  // NOLINT
/// Force the compiler to emit the code instead of performing a function call.
#define ATTR_ALWAYS_INLINE __attribute__ ((always_inline))  // NOLINT
/// Change the ABI to `fastcall`.
#define ATTR_FASTCALL __attribute__ ((fastcall))  // NOLINT
/// Avoid the compiler from generating function prologue and epilogue.
#define ATTR_NAKED __attribute__ ((naked))  // NOLINT
/// Mark the function as `noreturn`, this mean, the function should not return.
#define ATTR_NORETURN __attribute__ ((noreturn))  // NOLINT
/// \brief Make the compiler pass some parameters using registers instead of using the stack.
///
/// \param x the number of registers to be used as parameters.
#define ATTR_REGPARM(x) __attribute__ ((regparm (x)))  // NOLINT
/// \brief Change the attribute `section`, making the compiler to place the object in such section.
///
/// \param x the section where to emit the object.
#define ATTR_SECTION(x) __attribute__ ((section (x)))  // NOLINT
/// Avoid the compiler from optimizing-out functions that are actually used.
#define ATTR_USED __attribute__ ((used))  // NOLINT
/// \brief Change the visibility of the function.
///
/// \param x the new visibility.
#define ATTR_VISIBILITY(x) __attribute__ ((visibility(x)))  // NOLINT
/// Hint the compiler that a section of code is unreachable. Useful when a function is marked as `noreturn`.
#define BUILTIN_UNREACHABLE __builtin_unreachable()  // NOLINT
/// A *reordering barrier* to tell the compiler to not move statements around this barrier.
#define REORDERING_BARRIER __asm__ __volatile__("":::"memory")  // NOLINT

#endif

#else
#error "What compiler are you using?"
#endif

#endif //ZALEA_COMPILERMAGIC_H
