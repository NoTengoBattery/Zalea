//===-- LinkerScriptDefinitions.h - Macro Definitions for the Preprocessed Liker Script -------------------*- C -*-===//
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
/// Contains a batch of definitions that can be inserted in the linker scripts to be preprocessed, use them just like C
/// preprocessor macros.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_LINKERSCRIPTDEFINITIONS_H
#define ZALEA_LINKERSCRIPTDEFINITIONS_H
#include <config.h>

#if defined(KERNEL_ARM) || defined(KERNEL_x86)
#define MINI_ALIGN 0x10
#else
#error "Not a valid architecture."
#endif

#ifdef KERNEL_LINKER_GNU
#define _ENTRY ENTRY(_start)
#define _ASSERT_ADDRESS(x, y) . = x; \
ASSERT( x == ALIGN ( CONSTANT ( y ) ) , \
"This assertion was raised because the addresses from the current position need a special alignment." )
#define _ASSERT_LOAD_ADDRESS _ASSERT_ADDRESS(MACHINE_LOAD_ADDRESS, COMMONPAGESIZE)
#define _ASSERT_VIRTUAL_ADDRESS _ASSERT_ADDRESS(MACHINE_VIRTUAL_ADDRESS, COMMONPAGESIZE)
#define _SEGMENTSTART(x) . = SEGMENT_START ( x , MACHINE_VIRTUAL_ADDRESS );
#define _ALIGNED_CPS_SEGMENTSTART(x) . = SEGMENT_START ( x , ALIGN ( CONSTANT ( COMMONPAGESIZE ) ) );
#else
#error "Not a valid linker."
#endif

#endif //ZALEA_LINKERSCRIPTDEFINITIONS_H
