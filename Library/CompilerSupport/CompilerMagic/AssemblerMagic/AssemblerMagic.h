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

#ifndef __FILENAME__
#error "Please define the __FILENAME__ macro with the name of the current file before including this file."
#endif

#ifndef ASM_NL
#define ASM_NL ;
#endif

#define __MAGIC_INCLUDED__

    .file __FILENAME__

#if defined(KERNEL_x86)
#include <AssemblerMagic/x86/GnuAssemblerMagic.h>
#elif defined(KERNEL_ARM)
#include <AssemblerMagic/ARM/GnuAssemblerMagic.h>
#endif

#else
#error "What assembler are you using?"
#endif

#endif //ZALEA_ASSEMBLERMAGIC_H
