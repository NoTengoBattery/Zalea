#===-- x86.cmake - CMake Architecture File for x86 -----------------------------------------------------*- CMake -*-===#
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
#
# SPDX-License-Identifier: Apache-2.0
#
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ This file will define the general settings for the compiler. This includes the compiler itself (i.e. gcc, clang,
#/ etc), the binutils such as the assembler and linker and some other global options for the architecture. The
#/ machine file will probably override some options to be more specific to the machine.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This is a list of the *compilers* that are able to build this kernel for this architecture
SET(AVAILABLE_COMPILER "Clang" "GNU" "Intel")
CHECK_TOOL_BY_NAME(COMPILER "GNU")
# This is a list of *binutils* that are able to build the latest stages of this kernel for this architecture
SET(AVAILABLE_BINUTILS "GNU" "LLVM")
CHECK_TOOL_BY_NAME(BINUTILS "GNU")
# These lists will help CMake to build a target triple. The Toolchain script will test one by one until it founds one.
# The dash symbol means that it can be omitted, so we can have i.e. i386-pc-none and i386-pc
SET(ARCHS i386 i686 x86 8086 CACHE INTERNAL "-")
SET(SUBS - _64 CACHE INTERNAL "-")
SET(VENDORS - pc none CACHE INTERNAL "-")
SET(SYSS linux - none CACHE INTERNAL "-")
SET(ABIS gnu elf - CACHE INTERNAL "-")
# Change the CMake output format to ELF
SET(CMAKE_EXECUTABLE_FORMAT "ELF" CACHE INTERNAL "Executable file format")
# This is the default target for the compiler and binutils
SET_AND_EXPORT(KERNEL_TARGET "i686-elf" STRING "i686-elf"
               "This variable is the machine target for the compiler and binutils to generate code and executables.")
# This is the alternative target for the compiler and binutils (the host compiler in Linux distros)
SET_AND_EXPORT(KERNEL_ALTERNATIVE_TARGET "x86_64-linux-gnu" STRING "x86_64-linux-gnu"
               "This variable is the alternative machine target for the compiler and binutils to generate code.")
# This is the secondary target (LLVM needs this to correctly run the linker)
SET_AND_EXPORT(KERNEL_SECOND_TARGET "i686-linux-gnu" STRING "i686-linux-gnu"
               "This variable is the second machine target for the LLVM toolchain to generate code and executables.")
# This is the default ISA for the compiler and binutils (the minimum instruction set)
SET_AND_EXPORT(MACHINE_MARCH "pentium4" STRING "pentium4"
               "This variable is the machine minimum iteration of the ISA for the compiler and assembler to emit code.")
# This is the default CPU for the compiler and binutils to tune the performance
SET_AND_EXPORT(MACHINE_MTUNE "generic" STRING "generic"
               "This variable is the default CPU for the compiler and binutils to optimize and tune the performance.")
INCLUDE("${TREE_ARCHITECTURE_X_CONFIG_PATH}/ArchitectureExporter.cmake")
