#===-- x86.cmake - CMake Architecture file for x86 -----------------------------------------------------*- CMake -*-===#
#
# Copyright (c) 2020 Oever González
#
#  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
#  the License. You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
#  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
#  specific language governing permissions and limitations under the License.
# SPDX-License-Identifier: Apache-2.0
#
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ This file will define the general settings for the compiler. This includes the compiler itself (i.e. gcc, clang,
#/ msvc), the binutils such as the assembler and linker and some other "generic" options for the architecture. The
#/ respective machine file will probably override some options.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This is a list of the *compilers* that are able to build this kernel for this architecture
SET(AVAILABLE_COMPILER "Clang" "GNU")
CHECK_TOOL_BY_NAME(COMPILER "GNU")

# This is a list of *binutils* that are able to build the latest stages of this kernel for this architecture
SET(AVAILABLE_BINUTILS "GNU" "LLVM")
CHECK_TOOL_BY_NAME(BINUTILS "GNU")

# This is the default target for the compiler and binutils
SET_AND_EXPORT(KERNEL_TARGET "i686-elf" STRING "i686-elf"
               "This variable is the machine target for the compiler and binutils.")

# This is the default ISA for the compiler and binutils (the minimum instruction set)
SET_AND_EXPORT(MACHINE_MARCH "prescott" STRING "prescott"
               "This variable is the machine minimum iteration of the ISA for the compiler and binutils.")

# This is the default CPU for the compiler and binutils to tune the performance
SET_AND_EXPORT(MACHINE_MTUNE "generic" STRING "generic"
               "This variable is the default CPU for the compiler and binutils to tune the performance.")
