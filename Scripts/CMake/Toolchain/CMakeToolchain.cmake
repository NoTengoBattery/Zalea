#===-- CMakeToolchain.cmake - The Root of the Toolchain File for CMake Cross Compilation ---------------*- CMake -*-===#
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
#/ This file is the root of the Toolchain for CMake. This file will evaluate and configure the toolchain as it's
#/ detected.
#/
#===----------------------------------------------------------------------------------------------------------------===#

IF (TREE_SELF_PATH AND NOT TOOLCHAIN_DONE) # This will define if we have access to the scope variables and cache

  # Reset these variables... just in case something nasty happen to the cache.
  UNSET(KERNEL_LINKER_GNU CACHE)
  MARK_AS_ADVANCED(KERNEL_LINKER_GNU)
  UNSET(KERNEL_LINKER_GNU_BFD CACHE)
  MARK_AS_ADVANCED(KERNEL_LINKER_GNU_BFD)
  UNSET(KERNEL_LINKER_GNU_GOLD CACHE)
  MARK_AS_ADVANCED(KERNEL_LINKER_GNU_GOLD)
  UNSET(KERNEL_LINKER_LLVM_LLD CACHE)
  MARK_AS_ADVANCED(KERNEL_LINKER_LLVM_LLD)

  # Use GNU Gold as the linker (overrides the default linker)
  SET(KERNEL_USE_GOLD OFF CACHE BOOL "If this variable is on, the GNU gold linker will be used.")

  # Show these 3 variables in the cache
  SET(CMAKE_BINUTILS_PATH "" CACHE FILEPATH "A path where CMake can find the binutils tools.")
  SET(CMAKE_COMPILER_PATH "" CACHE FILEPATH "A path where CMake can find the compiler tools.")
  SET(CMAKE_EXTRA_TOOLS_PATH "" CACHE FILEPATH "A path where CMake can find it's extra tools.")

  # Set the target for the compiler (this is architecture specific and makes sense to Clang)
  SET(CMAKE_ASM_COMPILER_TARGET "${KERNEL_TARGET}")
  SET(CMAKE_C_COMPILER_TARGET "${KERNEL_TARGET}")
  SET(CMAKE_CXX_COMPILER_TARGET "${KERNEL_TARGET}")

  # Test the available targets to determine which one is available before giving up...
  SET(_KERNEL_TARGET "${KERNEL_TARGET}")
  FOREACH (_TEST_TARGET "${KERNEL_TARGET}" "${KERNEL_ALTERNATIVE_TARGET}" "${KERNEL_SECOND_TARGET}")
    SET(CMAKE_TEST_${_TEST_TARGET} "${_TEST_TARGET}-as")
    MESSAGE(STATUS "Trying to find the needed tools for the target '${_TEST_TARGET}'...")
    GUESS_TOOL_BY_NAME(TEST_${_TEST_TARGET} BINUTILS)
    GUESS_TOOL_BY_NAME(TEST_${_TEST_TARGET} COMPILER)
    IF (CMAKE_TEST_${_TEST_TARGET})
      SET(_KERNEL_TARGET "${_TEST_TARGET}")
      BREAK()
    ENDIF ()
  ENDFOREACH ()

  # Compiler selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_COMPILER}' compiler for the target "
          "'${_KERNEL_TARGET}'...")

  IF ("${KERNEL_COMPILER}" STREQUAL "Clang")
    SET(CMAKE_AR "llvm-ar")
    SET(CMAKE_ASM_COMPILER "clang")
    SET(CMAKE_ASM_COMPILER_AR "llvm-ar")
    SET(CMAKE_ASM_COMPILER_RANLIB "llvm-ranlib")
    SET(CMAKE_C_COMPILER "clang")
    SET(CMAKE_C_COMPILER_AR "llvm-ar")
    SET(CMAKE_C_COMPILER_RANLIB "llvm-ranlib")
    SET(CMAKE_CXX_COMPILER "clang++")
    SET(CMAKE_CXX_COMPILER_AR "llvm-ar")
    SET(CMAKE_CXX_COMPILER_RANLIB "llvm-ranlib")
  ELSEIF ("${KERNEL_COMPILER}" STREQUAL "GNU"
          OR "${KERNEL_COMPILER}" STREQUAL "Intel")
    SET(CMAKE_AR "${_KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_ASM_COMPILER_AR "${_KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_ASM_COMPILER_RANLIB "${_KERNEL_TARGET}-gcc-ranlib")
    SET(CMAKE_C_COMPILER_AR "${_KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_C_COMPILER_RANLIB "${_KERNEL_TARGET}-gcc-ranlib")
    SET(CMAKE_CXX_COMPILER_AR "${_KERNEL_TARGET}-gcc-ar")
    SET(CMAKE_CXX_COMPILER_RANLIB "${_KERNEL_TARGET}-gcc-ranlib")
    IF ("${KERNEL_COMPILER}" STREQUAL "GNU")
      SET(CMAKE_ASM_COMPILER "${_KERNEL_TARGET}-gcc")
      SET(CMAKE_C_COMPILER "${_KERNEL_TARGET}-gcc")
      SET(CMAKE_CXX_COMPILER "${_KERNEL_TARGET}-g++")
    ELSE ()
      SET(CMAKE_ASM_COMPILER "icc")
      SET(CMAKE_C_COMPILER "icc")
      SET(CMAKE_CXX_COMPILER "icpc")
    ENDIF ()
  ENDIF ()

  GUESS_TOOL_BY_NAME(AR BINUTILS)
  GUESS_TOOL_BY_NAME(AR COMPILER)
  GUESS_TOOL_BY_NAME(ASM_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_AR BINUTILS)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_RANLIB BINUTILS)
  GUESS_TOOL_BY_NAME(ASM_COMPILER_RANLIB COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER_AR BINUTILS)
  GUESS_TOOL_BY_NAME(C_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(C_COMPILER_RANLIB BINUTILS)
  GUESS_TOOL_BY_NAME(C_COMPILER_RANLIB COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_AR BINUTILS)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_AR COMPILER)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_RANLIB BINUTILS)
  GUESS_TOOL_BY_NAME(CXX_COMPILER_RANLIB COMPILER)

  # Binutils selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_BINUTILS}' binutils for the  target "
          "'${_KERNEL_TARGET}'...")

  IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
    SET(CMAKE_LD "ld.lld")
    SET(CMAKE_NM "llvm-nm")
    SET(CMAKE_OBJCOPY "llvm-objcopy")
    SET(CMAKE_OBJDUMP "llvm-objdump")
    SET(CMAKE_RANLIB "llvm-ranlib")
    SET(CMAKE_READELF "llvm-readelf")
    SET(CMAKE_LD_NAME "lld")
  ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
    SET(CMAKE_LD "${_KERNEL_TARGET}-ld")
    SET(CMAKE_NM "${_KERNEL_TARGET}-nm")
    SET(CMAKE_OBJCOPY "${_KERNEL_TARGET}-objcopy")
    SET(CMAKE_OBJDUMP "${_KERNEL_TARGET}-objdump")
    SET(CMAKE_RANLIB "${_KERNEL_TARGET}-ranlib")
    SET(CMAKE_READELF "${_KERNEL_TARGET}-readelf")
    SET(CMAKE_LD_NAME "bfd")
  ENDIF ()
  IF (KERNEL_USE_GOLD)
    SET(CMAKE_LD "${_KERNEL_TARGET}-ld.gold")
    SET(CMAKE_LD_NAME "gold")
  ENDIF ()

  FORCE_TOOL_BY_NAME(LD BINUTILS)
  FORCE_TOOL_BY_NAME(NM BINUTILS)
  FORCE_TOOL_BY_NAME(OBJCOPY BINUTILS)
  FORCE_TOOL_BY_NAME(OBJDUMP BINUTILS)
  FORCE_TOOL_BY_NAME(RANLIB BINUTILS)
  FORCE_TOOL_BY_NAME(READELF BINUTILS)

  # CMake supports CCache, therefore we can detect it and use it :)
  SET(CMAKE_CCACHE "ccache")
  GUESS_TOOL_BY_NAME(CCACHE BINUTILS)
  GUESS_TOOL_BY_NAME(CCACHE COMPILER)
  IF (CMAKE_CCACHE)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "\"${CMAKE_CCACHE}\"")
  ENDIF ()

  # Try to find clang-tidy, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CLANG_TIDY "clang-tidy")
  GUESS_TOOL_BY_NAME(CLANG_TIDY BINUTILS)
  GUESS_TOOL_BY_NAME(CLANG_TIDY COMPILER)
  GUESS_TOOL_BY_NAME(CLANG_TIDY EXTRA_TOOLS)

  # Try to find cpplint, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CPPLINT "cpplint")
  GUESS_TOOL_BY_NAME(CPPLINT BINUTILS)
  GUESS_TOOL_BY_NAME(CPPLINT COMPILER)
  GUESS_TOOL_BY_NAME(CPPLINT EXTRA_TOOLS)

  # Try to find cppcheck, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CPPCHECK "cppcheck")
  GUESS_TOOL_BY_NAME(CPPCHECK BINUTILS)
  GUESS_TOOL_BY_NAME(CPPCHECK COMPILER)
  GUESS_TOOL_BY_NAME(CPPCHECK EXTRA_TOOLS)

  # Export which linker is being used to the database, so we can use it in the preprocessed linker scripts
  IF (KERNEL_USE_GOLD)
    SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
    SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU_GOLD ON BOOL ON "-")
  ELSE ()
    IF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU_BFD ON BOOL ON "-")
    ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_GNU ON BOOL ON "-")
      SET_AND_EXPORT_FORCE(KERNEL_LINKER_LLVM_LLD ON BOOL ON "-")
    ENDIF ()
  ENDIF ()

  # When using a GNU compatible driver, this will tell the compiler where to find it's binutils (mainly the linker)
  IF ("${KERNEL_COMPILER}" STREQUAL "Clang"
      OR "${KERNEL_COMPILER}" STREQUAL "Intel"
      OR "${KERNEL_COMPILER}" STREQUAL "GNU")
    GET_FILENAME_COMPONENT(CMAKE_BINUTILS_BIN_PATH "${CMAKE_LD}" DIRECTORY)
    SET(CMAKE_BINUTILS_BIN_PATH "${CMAKE_BINUTILS_BIN_PATH}" CACHE INTERNAL "Absolute path to the binutils.")
  ENDIF ()

  SET(TOOLCHAIN_DONE ON)

ENDIF ()
