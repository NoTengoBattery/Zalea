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

  # Use GNU Gold as the linker (overrides the default linker)
  SET(KERNEL_USE_GOLD OFF CACHE BOOL "If this variable is on, the GNU gold linker will be used.")

  # Show these 3 variables in the cache
  SET(CMAKE_BINUTILS_PATH "" CACHE FILEPATH "A path where CMake can find the binutils tools.")
  SET(CMAKE_COMPILER_PATH "" CACHE FILEPATH "A path where CMake can find the compiler tools.")
  SET(CMAKE_EXTRA_TOOLS_PATH "" CACHE FILEPATH "A path where CMake can find it's extra tools.")

  # Set the target for the compiler (this is architecture specific and makes sense to Clang)
  SET(CMAKE_ASM_COMPILER_TARGET "${KERNEL_SECOND_TARGET}")
  SET(CMAKE_C_COMPILER_TARGET "${KERNEL_SECOND_TARGET}")
  SET(CMAKE_CXX_COMPILER_TARGET "${KERNEL_SECOND_TARGET}")

  # Test the available targets to determine which one is available before giving up...
  SET(_KERNEL_TARGET "${KERNEL_SECOND_TARGET}")
  TARGET_TRIPLE_LIST(ALL_POSSIBLE_TRIPLES)
  FOREACH (_TEST_TARGET ${ALL_POSSIBLE_TRIPLES})
    SET(CMAKE_TOOLS_FOR_${_TEST_TARGET} "${_TEST_TARGET}-objcopy;${_TEST_TARGET}-g++")
    GUESS_TOOL_BY_NAME(TOOLS_FOR_${_TEST_TARGET} "COMPILER;BINUTILS;EXTRA_TOOLS")
    IF (CMAKE_TOOLS_FOR_${_TEST_TARGET})
      UNSET(CMAKE_TOOLS_FOR_${_TEST_TARGET} CACHE)
      SET(_KERNEL_TARGET "${_TEST_TARGET}")
      MESSAGE(STATUS "Found CMake tools for the following target: '${_TEST_TARGET}'")
      BREAK()
    ENDIF ()
    UNSET(CMAKE_TOOLS_FOR_${_TEST_TARGET} CACHE)
  ENDFOREACH ()

  # Compiler selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_COMPILER}' compiler for the target "
          "'${_KERNEL_TARGET}'...")

  IF ("${KERNEL_COMPILER}" STREQUAL "Clang")
    SET(CMAKE_AR "llvm-ar;ar")
    SET(CMAKE_RANLIB "llvm-ranlib;llvm-ar;ranlib")
    SET(CMAKE_ASM_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_ASM_COMPILER_RANLIB "${CMAKE_RANLIB}")
    SET(CMAKE_C_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_C_COMPILER_RANLIB "${CMAKE_RANLIB}")
    SET(CMAKE_CXX_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_CXX_COMPILER_RANLIB "${CMAKE_RANLIB}")
    SET(CMAKE_ASM_COMPILER "clang;clang++")
    SET(CMAKE_C_COMPILER "clang;clang++")
    SET(CMAKE_CXX_COMPILER "clang++;clang")
  ELSEIF ("${KERNEL_COMPILER}" STREQUAL "GNU"
          OR "${KERNEL_COMPILER}" STREQUAL "Intel")
    SET(CMAKE_AR "${_KERNEL_TARGET}-gcc-ar;${_KERNEL_TARGET}-ar;gcc-ar;ar")
    SET(CMAKE_RANLIB "${_KERNEL_TARGET}-gcc-ranlib;${_KERNEL_TARGET}-ranlib;gcc-ranlib;ranlib")
    IF ("${KERNEL_COMPILER}" STREQUAL "GNU")
      SET(CMAKE_ASM_COMPILER "${_KERNEL_TARGET}-gcc;gcc")
      SET(CMAKE_C_COMPILER "${_KERNEL_TARGET}-gcc;gcc")
      SET(CMAKE_CXX_COMPILER "${_KERNEL_TARGET}-g++;${_KERNEL_TARGET}-c++;g++;c++")
    ELSE ()
      SET(CMAKE_AR "xiar;${CMAKE_AR}")
      SET(CMAKE_ASM_COMPILER "icc;icpc")
      SET(CMAKE_C_COMPILER "icc;icpc")
      SET(CMAKE_CXX_COMPILER "icpc;icc")
    ENDIF ()
    SET(CMAKE_ASM_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_ASM_COMPILER_RANLIB "${CMAKE_RANLIB}")
    SET(CMAKE_C_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_C_COMPILER_RANLIB "${CMAKE_RANLIB}")
    SET(CMAKE_CXX_COMPILER_AR "${CMAKE_AR}")
    SET(CMAKE_CXX_COMPILER_RANLIB "${CMAKE_RANLIB}")
  ENDIF ()
  GUESS_TOOL_BY_NAME(AR "BINUTILS;COMPILER;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(RANLIB "BINUTILS;COMPILER;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(ASM_COMPILER_AR "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(ASM_COMPILER_RANLIB "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(C_COMPILER_AR "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(C_COMPILER_RANLIB "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(CXX_COMPILER_AR "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(CXX_COMPILER_RANLIB "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(ASM_COMPILER "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(C_COMPILER "COMPILER;BINUTILS;EXTRA_TOOLS")
  GUESS_TOOL_BY_NAME(CXX_COMPILER "COMPILER;BINUTILS;EXTRA_TOOLS")

  # Binutils selection logic...
  MESSAGE(STATUS "CMake is attempting to auto configure the '${KERNEL_BINUTILS}' binutils for the  target "
          "'${_KERNEL_TARGET}'...")

  IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
    SET(CMAKE_LD "ld.lld")
    SET(CMAKE_LINKER "${CMAKE_LD}")
    SET(CMAKE_NM "llvm-nm;nm")
    SET(CMAKE_OBJCOPY "llvm-objcopy;objcopy")
    SET(CMAKE_OBJDUMP "llvm-objdump;objdump")
    SET(CMAKE_READELF "llvm-readelf;llvm-readobj;readelf")
    SET(CMAKE_LD_NAME "lld")
  ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
    SET(CMAKE_LD "${_KERNEL_TARGET}-ld;${_KERNEL_TARGET}-ld.bfd;ld;ld.bfd")
    IF ("${KERNEL_COMPILER}" STREQUAL "Intel")
      SET(CMAKE_LD "xild;${CMAKE_LD}")
    ENDIF ()
    SET(CMAKE_LINKER "${CMAKE_LD}")
    SET(CMAKE_NM "${_KERNEL_TARGET}-gcc-nm;${_KERNEL_TARGET}-nm;gcc-nm;nm")
    SET(CMAKE_OBJCOPY "${_KERNEL_TARGET}-objcopy;objcopy")
    SET(CMAKE_OBJDUMP "${_KERNEL_TARGET}-objdump;objdump")
    SET(CMAKE_READELF "${_KERNEL_TARGET}-readelf;readelf")
    SET(CMAKE_LD_NAME "bfd")
  ENDIF ()
  IF (KERNEL_USE_GOLD)
    SET(CMAKE_LD "${_KERNEL_TARGET}-ld.gold;${_KERNEL_TARGET}-gold;ld.gold;gold")
    SET(CMAKE_LD_NAME "gold")
  ENDIF ()

  FORCE_TOOL_BY_NAME(LD "BINUTILS;COMPILER;EXTRA_TOOLS")
  FORCE_TOOL_BY_NAME(LINKER "BINUTILS;COMPILER;EXTRA_TOOLS")
  FORCE_TOOL_BY_NAME(NM "BINUTILS;COMPILER;EXTRA_TOOLS")
  FORCE_TOOL_BY_NAME(OBJCOPY "BINUTILS;COMPILER;EXTRA_TOOLS")
  FORCE_TOOL_BY_NAME(OBJDUMP "BINUTILS;COMPILER;EXTRA_TOOLS")
  FORCE_TOOL_BY_NAME(READELF "BINUTILS;COMPILER;EXTRA_TOOLS")

  MESSAGE(STATUS "CMake is attempting to auto configure the optional tools...")

  # Try to find GDB or LLDB
  SET(CMAKE_DEBUGGER "${_KERNEL_TARGET}-gdb;gdb;lldb")
  GUESS_TOOL_BY_NAME(DEBUGGER "BINUTILS;COMPILER;EXTRA_TOOLS")

  # Try to find QEMU system
  SET(CMAKE_QEMU "qemu-system")
  FOREACH (ARCHITECTURE_NAME ${ARCHITECTURE_NAMES})
    LIST(APPEND CMAKE_QEMU "qemu-system-${ARCHITECTURE_NAME}")
  ENDFOREACH ()
  GUESS_TOOL_BY_NAME(QEMU "EXTRA_TOOLS;BINUTILS;COMPILER")

  # CMake supports CCache, therefore we can detect it and use it :)
  SET(CMAKE_CCACHE "ccache")
  GUESS_TOOL_BY_NAME(CCACHE "EXTRA_TOOLS;BINUTILS;COMPILER")
  IF (CMAKE_CCACHE)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "\"${CMAKE_CCACHE}\"")
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK "\"${CMAKE_CCACHE}\"")
  ENDIF ()

  # Try to find clang-tidy, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CLANG_TIDY "clang-tidy")
  GUESS_TOOL_BY_NAME(CLANG_TIDY "COMPILER;BINUTILS;EXTRA_TOOLS")

  # Try to find cpplint, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CPPLINT "cpplint")
  GUESS_TOOL_BY_NAME(CPPLINT "EXTRA_TOOLS;BINUTILS;COMPILER")

  # Try to find cppcheck, if available, set the property in the main CMakeLists.txt
  SET(CMAKE_CPPCHECK "cppcheck")
  GUESS_TOOL_BY_NAME(CPPCHECK "EXTRA_TOOLS;BINUTILS;COMPILER")

  # Export which linker is being used to the database, so we can use it in the preprocessed linker scripts
  IF (KERNEL_USE_GOLD)
    SET_AND_EXPORT(KERNEL_LINKER_GNU ON INTERNAL ON "-")
    SET_AND_EXPORT(KERNEL_LINKER_GNU_GOLD ON INTERNAL ON "-")
  ELSE ()
    IF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
      SET_AND_EXPORT(KERNEL_LINKER_GNU ON INTERNAL ON "-")
      SET_AND_EXPORT(KERNEL_LINKER_GNU_BFD ON INTERNAL ON "-")
    ELSEIF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
      SET_AND_EXPORT(KERNEL_LINKER_GNU ON INTERNAL ON "-")
      SET_AND_EXPORT(KERNEL_LINKER_LLVM_LLD ON INTERNAL ON "-")
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
