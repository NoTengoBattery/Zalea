#===-- Kernel-GNU-ASM.cmake - CMake System-Compiler-Language File ====----------------------------------*- CMake -*-===#
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
#/ This file contains System-specific, Compiler-specific and Language-specific code and variables. This file is called
#/ just after CMake identifies the compiler, and before the Compiler-specific, Language-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===----------------------------------------------------------------------------------------------------------------===#

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  # If the user gave the system a path for binutils, tell the driver to use it first
  IF (CMAKE_BINUTILS_BIN_PATH)
    STRING(APPEND CMAKE_ASM_FLAGS_INIT "\"-B${CMAKE_BINUTILS_BIN_PATH}\" ")
  ENDIF ()

  # Those are the base "freestanding" flags
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-ffreestanding ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-nostdlib ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-pedantic ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-pipe ")
  # Those flags define part of the ABI that is common to all architectures
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-fno-delete-null-pointer-checks ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-fstack-protector-strong -fstack-clash-protection ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-fsanitize=kernel-address ")
  # Those flags define the diagnostics to be issued (or not) by the compiler
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Wall ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Werror ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Wextra ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Wformat=2 ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Wpedantic ")
  # Those flags define the linker to be used (this is needed for all cross compilers)
  STRING(APPEND CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=\"${CMAKE_LD_NAME}\" ")
  # Add the march and mtune flags
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-march=${MACHINE_MARCH} ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mtune=${MACHINE_MTUNE} ")

  # These flags are based on which kind of build we are doing
  STRING(APPEND CMAKE_ASM_FLAGS_DEBUG_INIT "-g -DDEBUG ")
  STRING(APPEND CMAKE_ASM_FLAGS_MINSIZEREL_INIT "-Os -DMINSIZEREL ")
  STRING(APPEND CMAKE_ASM_FLAGS_RELEASE_INIT "-O3 -DRELEASE ")
  STRING(APPEND CMAKE_ASM_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -DRELWITHDEBINFO ")

  # Options for the CMake compiler
  SET(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "sx")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-x assembler-with-cpp ")

ENDIF ()
