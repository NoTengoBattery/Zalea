#===-- CMakeToolchain.cmake - The root of the toolchain file for CMake cross compilation  -------------*- CMake -*-===//
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
#/ This file is the root of the toolchain architecture for CMake. This file will evaluate and configure the toolchain as
#/ it's detected.
#/
#===---------------------------------------------------------------------------------------------------------------===//

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  # Compiler selection logic...
  MESSAGE(STATUS "The CMake Toolchain File is attempting to auto configure the '${KERNEL_COMPILER}' compiler for the "
          "target '${KERNEL_TARGET}'...")
  IF ("${KERNEL_COMPILER}" STREQUAL "Clang")
    SET(CMAKE_ASM_COMPILER "clang")
    SET(CMAKE_ASM_COMPILER_TARGET "${KERNEL_TARGET}")
    SET(CMAKE_C_COMPILER "clang")
    SET(CMAKE_C_COMPILER_TARGET "${KERNEL_TARGET}")
    SET(CMAKE_CXX_COMPILER "clang++")
    SET(CMAKE_CXX_COMPILER_TARGET "${KERNEL_TARGET}")
  ELSEIF ("${KERNEL_COMPILER}" STREQUAL "GNU")
    SET(CMAKE_ASM_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_C_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_CXX_COMPILER "${KERNEL_TARGET}-g++")
  ENDIF ()

  GUESS_TOOL_BY_NAME(ASM_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER COMPILER)

  # Ensure this lock. This way, the compiler name (or path) is in the path in the next run
  IF (NOT "$CACHE{_COMPILER_LOCK}")
    SET(_COMPILER_LOCK "ON" CACHE INTERNAL "This lock will force the user to re-run CMake after the compiler toolchain")
    MESSAGE(FATAL_ERROR "The CMake was changed by the CMake Toolchain file. In order to apply the changes, you will "
            "need to run CMake again.")
  ENDIF ()

ENDIF ()
