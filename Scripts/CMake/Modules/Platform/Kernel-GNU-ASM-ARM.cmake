#===-- Kernel-GNU-ASM-ARM.cmake - CMake System-Compiler-Language-Architecture file ====----------------*- CMake -*-===//
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
#/ This file contains System-specific, Compiler-specific, Language-specific and Architecture-specific code and
#/ variables. This file is called just after CMake identifies the compiler, and before the Compiler-specific,
#/ Language-specific configuration file when CMake checks for a "working compiler".
#/
#===---------------------------------------------------------------------------------------------------------------===//

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  SET(CMAKE_ASM_FLAGS_INIT)

  # Those are the base "machine" and "architecture" flags
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-march=${MACHINE_MARCH} ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mtune=${MACHINE_MTUNE} ")

  # Those are special base flags which will setup the ABI for the compiler
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mabi=aapcs ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mfloat-abi=soft ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mgeneral-regs-only ")
  STRING(APPEND CMAKE_ASM_FLAGS_INIT "-mtp=soft ")

  # TODO: marm and mthumb for minsizerel

ENDIF ()