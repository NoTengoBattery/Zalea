#===-- CMakeToolchain.cmake - The root of the toolchain file for CMake cross compilation  --------------*- CMake -*-===#
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
#/ This file is the root of the toolchain architecture for CMake. This file will evaluate and configure the toolchain as
#/ it's detected.
#/
#===----------------------------------------------------------------------------------------------------------------===#

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  # Reset these variables... just in case something nasty happen to the cache.
  UNSET(KERNEL_LINKER_GNU CACHE)
  UNSET(KERNEL_LINKER_GNU_BFD CACHE)
  UNSET(KERNEL_LINKER_GNU_GOLD CACHE)
  UNSET(KERNEL_LINKER_LLVM_LLD CACHE)

  # Use GNU Gold as the linker (overrides the default linker)
  SET(KERNEL_USE_GOLD "OFF" CACHE BOOL "If this variable is on, the GNU gold linker will be used.")
  MARK_AS_ADVANCED(KERNEL_USE_GOLD)

  # Set the target for the compiler (this is architecture specific and makes sense to Clang)
  SET(CMAKE_ASM_COMPILER_TARGET "${KERNEL_TARGET}")
  SET(CMAKE_C_COMPILER_TARGET "${KERNEL_TARGET}")
  SET(CMAKE_CXX_COMPILER_TARGET "${KERNEL_TARGET}")

  # Compiler selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_COMPILER}' compiler for the target "
          "'${KERNEL_TARGET}'...")

  IF ("${KERNEL_COMPILER}" STREQUAL "Clang")
    SET(CMAKE_ASM_COMPILER "clang")
    SET(CMAKE_ASM_COMPILER_AR "llvm-ar")
    SET(CMAKE_ASM_COMPILER_RANLIB "llvm-ranlib")
    SET(CMAKE_C_COMPILER "clang")
    SET(CMAKE_C_COMPILER_AR "llvm-ar")
    SET(CMAKE_C_COMPILER_RANLIB "llvm-ranlib")
    SET(CMAKE_CXX_COMPILER "clang++")
    SET(CMAKE_CXX_COMPILER_AR "llvm-ar")
    SET(CMAKE_CXX_COMPILER_RANLIB "llvm-ranlib")
  ELSEIF ("${KERNEL_COMPILER}" STREQUAL "GNU")
    SET(CMAKE_ASM_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_ASM_COMPILER_AR "${KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_ASM_COMPILER_RANLIB "${KERNEL_TARGET}-gcc-ranlib")
    SET(CMAKE_C_COMPILER "${KERNEL_TARGET}-gcc")
    SET(CMAKE_C_COMPILER_AR "${KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_C_COMPILER_RANLIB "${KERNEL_TARGET}-gcc-ranlib")
    SET(CMAKE_CXX_COMPILER "${KERNEL_TARGET}-g++")
    SET(CMAKE_CXX_COMPILER_AR "${KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_CXX_COMPILER_RANLIB "${KERNEL_TARGET}-gcc-ranlib")
  ENDIF ()

  GUESS_TOOL_BY_NAME(ASM_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_RANLIB COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER_RANLIB COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_RANLIB COMPILER)

  # Binutils selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_BINUTILS}' binutils for the  target "
          "'${KERNEL_TARGET}'...")

  IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
    SET(CMAKE_AR "llvm-ar")
    SET(CMAKE_LD "ld.lld")
    SET(CMAKE_OBJCOPY "llvm-objcopy")
    SET(CMAKE_OBJDUMP "llvm-objdump")
    SET(CMAKE_RANLIB "llvm-ranlib")
    SET(CMAKE_LD_NAME "lld")
  ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
    SET(CMAKE_AR "${KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_LD "${KERNEL_TARGET}-ld")
    SET(CMAKE_OBJCOPY "${KERNEL_TARGET}-objcopy")
    SET(CMAKE_OBJDUMP "${KERNEL_TARGET}-objdump")
    SET(CMAKE_RANLIB "${KERNEL_TARGET}-gcc-ranlib")
    SET(CMAKE_LD_NAME "bfd")
  ENDIF ()
  IF (KERNEL_USE_GOLD)
    SET(CMAKE_LD "${KERNEL_TARGET}-ld.gold")
    SET(CMAKE_LD_NAME "gold")
  ENDIF ()

  FORCE_TOOL_BY_NAME(AR BINUTILS)
  FORCE_TOOL_BY_NAME(LD BINUTILS)
  FORCE_TOOL_BY_NAME(OBJCOPY BINUTILS)
  FORCE_TOOL_BY_NAME(OBJDUMP BINUTILS)
  FORCE_TOOL_BY_NAME(RANLIB BINUTILS)

  # Export which linker is being used to the database, so we can use it in the preprocessed linker scripts
  IF (KERNEL_USE_GOLD)
    SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
    SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU_GOLD ON BOOL ON "-")
    MARK_AS_ADVANCED(KERNEL_LINKER_GNU)
    MARK_AS_ADVANCED(KERNEL_LINKER_GNU_GOLD)
  ELSE ()
    IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_LLVM_LLD ON BOOL ON "-")
      MARK_AS_ADVANCED(KERNEL_LINKER_GNU)
      MARK_AS_ADVANCED(KERNEL_LINKER_LLVM_LLD)
    ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU_BFD ON BOOL ON "-")
      MARK_AS_ADVANCED(KERNEL_LINKER_GNU)
      MARK_AS_ADVANCED(KERNEL_LINKER_GNU_BFD)
    ENDIF ()
  ENDIF ()

  # When using Clang or GCC, this will tell the compiler where to find it's binutils (mainly the linker)
  GET_FILENAME_COMPONENT(CMAKE_BINUTILS_BIN_PATH "${CMAKE_LD}" DIRECTORY)
  SET(CMAKE_BINUTILS_BIN_PATH "${CMAKE_BINUTILS_BIN_PATH}" CACHE INTERNAL
      "Absolute path from the C compiler to the binutils.")

ENDIF ()
