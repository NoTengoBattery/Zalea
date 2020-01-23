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

  SET(KERNEL_USE_GOLD "OFF" CACHE BOOL "If this variable is on, the GNU gold linker will be used.")

  # Compiler selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_COMPILER}' compiler for the target "
          "'${KERNEL_TARGET}'...")
  IF ("${KERNEL_COMPILER}" STREQUAL "Clang")
    SET(CMAKE_ASM_COMPILER "clang")
    SET(CMAKE_C_COMPILER "clang")
    SET(CMAKE_CXX_COMPILER "clang++")
    SET(CMAKE_ASM_COMPILER_TARGET "${KERNEL_TARGET}")
    SET(CMAKE_C_COMPILER_TARGET "${KERNEL_TARGET}")
    SET(CMAKE_CXX_COMPILER_TARGET "${KERNEL_TARGET}")
  ELSEIF ("${KERNEL_COMPILER}" STREQUAL "GNU")
    SET(CMAKE_ASM_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_C_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_CXX_COMPILER "${KERNEL_TARGET}-g++")
  ENDIF ()
  GUESS_TOOL_BY_NAME(ASM_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER COMPILER)

  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_BINUTILS}' binutils for the  target "
          "'${KERNEL_TARGET}'...")
  IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
    SET(CMAKE_LD "ld.lld")
    SET(CMAKE_LD_NAME "lld")
    SET(CMAKE_OBJCOPY "llvm-objcopy")
  ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
    SET(CMAKE_LD "${KERNEL_TARGET}-ld")
    SET(CMAKE_LD_NAME "bfd")
    SET(CMAKE_OBJCOPY "${KERNEL_TARGET}-objcopy")
  ENDIF ()
  IF (KERNEL_USE_GOLD)
    SET(CMAKE_LD "${KERNEL_TARGET}-ld.gold")
    SET(CMAKE_LD_NAME "gold")
  ENDIF ()
  FORCE_TOOL_BY_NAME(LD BINUTILS)
  FORCE_TOOL_BY_NAME(OBJCOPY BINUTILS)

  GET_FILENAME_COMPONENT(CMAKE_BINUTILS_BIN_PATH "${CMAKE_LD}" DIRECTORY)
  GET_FILENAME_COMPONENT(CMAKE_CC_BIN_PATH "${CMAKE_CXX_COMPILER}" DIRECTORY)
  FILE(RELATIVE_PATH CMAKE_BINUTILS_BIN_PATH "${CMAKE_CC_BIN_PATH}" "${CMAKE_BINUTILS_BIN_PATH}")
  IF ("${CMAKE_BINUTILS_BIN_PATH}" STREQUAL "")
    SET(CMAKE_BINUTILS_BIN_PATH ".")
  ENDIF ()
  SET(CMAKE_BINUTILS_BIN_PATH "${CMAKE_BINUTILS_BIN_PATH}" CACHE INTERNAL
      "Relative path from the C compiler to the binutils")

ENDIF ()
