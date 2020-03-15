/*===-- LinkerScriptDefinitions.in - Macro Definitions for the Preprocessed Liker Script -------------------*- C -*-===*
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
 * Contains a batch of definitions that can be inserted in the linker scripts to be preprocessed, use them just like C
 * preprocessor macros.
 *
 **********************************************************************************************************************/

#ifndef ZALEA_LINKERSCRIPTDEFINITIONS_IN
#define ZALEA_LINKERSCRIPTDEFINITIONS_IN

#include <config.h>

#ifndef __LD__
#error "A required macro is not set, so you're probably not pre-processing a linker script."
#endif

#if defined(KERNEL_ARM) || defined(KERNEL_x86)
#define MINI_ALIGN 0x10 /* 128 bit (16 bytes) */
#define MICRO_ALIGN 0x08 /* 64 bit (8 bytes) */
#else
#error "Not a valid architecture."
#endif

#ifdef KERNEL_LINKER_GNU
#define ENTRY ENTRY( start )
#define ALIGN_COMMONPAGE ALIGN ( CONSTANT ( COMMONPAGESIZE ) )
#define ALIGN_MAXPAGESIZE ALIGN ( CONSTANT ( MAXPAGESIZE ) )
#define ALIGN_MINI ALIGN ( MINI_ALIGN )
#define ALIGN_MICRO ALIGN ( MICRO_ALIGN )
#define ASSERT_ADDRESS(x, y) ASSERT ( x % CONSTANT ( COMMONPAGESIZE ) == 0, \
"The value of this variable is required to be aligned to the common page size. Aborting the linking now." )
#define ASSERT_LOAD_ADDRESS ASSERT_ADDRESS(MACHINE_LOAD_ADDRESS, ALIGN_COMMONPAGE) /* MACHINE_LOAD_ADDRESS */
#define ASSERT_VIRTUAL_ADDRESS ASSERT_ADDRESS(MACHINE_VIRTUAL_ADDRESS, ALIGN_COMMONPAGE) /* MACHINE_VIRTUAL_ADDRESS */
#define ASSERT_STACK_SIZE ASSERT_ADDRESS(MACHINE_STACK_SIZE, ALIGN_COMMONPAGE) /* MACHINE_STACK_SIZE */
#define _SEGMENT_START(x) . = MACHINE_VIRTUAL_ADDRESS; /*
 */ . = SEGMENT_START ( x , MACHINE_VIRTUAL_ADDRESS ); /* MACHINE_VIRTUAL_ADDRESS */
#define ALIGNED_CPS_SEGMENT_START(x) . = ALIGN_COMMONPAGE; /*
 */ . = SEGMENT_START ( x , ALIGN_COMMONPAGE );
#define PF_X 0x01 /* Executable */
#define PF_W 0x02 /* Writable */
#define PF_R 0x04 /* Readable */
#define PF_RX 0x05 /* Readable + Executable */
#define PF_RW 0x06 /* Readable + Writable */
#else
#error "Not a valid linker."
#endif

#endif //ZALEA_LINKERSCRIPTDEFINITIONS_IN
