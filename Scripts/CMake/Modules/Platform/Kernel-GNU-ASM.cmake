#===-- Kernel-GNU-ASM.cmake - CMake System-Compiler-Language file ====---------------------------------*- CMake -*-===//
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
#===---------------------------------------------------------------------------------------------------------------===//
#/
#/ \file
#/ This file contains System-specific, Compiler-specific and Language-specific code and variables. This file is called
#/ just after CMake identifies the compiler, and before the Compiler-specific, Language-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===---------------------------------------------------------------------------------------------------------------===//

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  # If the user gave the system a path for binutils, tell the driver to use it first
  IF (CMAKE_BINUTILS_BIN_PATH)
    STRING(APPEND CMAKE_ASM_FLAGS_INIT "\"-B${CMAKE_BINUTILS_BIN_PATH}\" ")
  ENDIF ()

  # Those are the base "freestanding" flags
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-ffreestanding ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-nostdlib ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-pipe ")
  # Those flags define the diagnostics to be issued (or not) by the compiler
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-Wall -Wextra -Wpedantic -Wformat=2 -Werror ")
  # This flag defines the linker to be used (this is needed for all cross compilers)
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-fuse-ld=${CMAKE_LD_NAME}")

ENDIF ()
