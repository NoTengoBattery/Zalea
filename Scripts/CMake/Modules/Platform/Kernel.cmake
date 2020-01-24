#===--Kernel.cmake - CMake System File  ----------------------------------------------------------------*- CMake -*-===#
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
#/ This file contains System-specific, Compiler-generic and Language-generic code and variables. This file is called
#/ just after CMake identifies the compiler, and before the Compiler-specific, Language-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===----------------------------------------------------------------------------------------------------------------===#

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  # Check some special cases for compiler IDs
  IF (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "${KERNEL_COMPILER}")
    # Let AppleClang go on! We support AppleClang
    IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      MESSAGE(STATUS "Using Apple's Clang instead of vanilla Clang/LLVM...")
    ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
      # Stop MSVC and say why!
      MESSAGE(FATAL_ERROR "The Microsoft's MSVC compiler is not supported by this project.\n"
              "This is because, even with a valid set of binutils, the compiler can't be configured to avoid "
              " generating floating point instructions, among other assumptions that are useful for the Microsoft's "
              "ABI, but useless or harmful for this project's ABI.")
    ELSE ()
      MESSAGE(FATAL_ERROR "The compiler ID inferred by CMake ('${CMAKE_CXX_COMPILER_ID}') is not the same as the "
              " compiler ID selected by the user ('${KERNEL_COMPILER}') . This means that CMake ran a fallback "
              "algorithm and didn't respected (or found) the requested compiler. Please install and configure the "
              "compiler correctly.")
    ENDIF ()
  ENDIF ()

  # When using Clang (but not AppleClang), version must have to be 9.0 or greater
  IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
      AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    MESSAGE(FATAL_ERROR "When using the Clang compiler, Clang's version must have to be 9.0 or greater.")
  ENDIF ()

  # When using AppleClang, version must have to be 11.0.0 or greater
  IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"
      AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0.0)
    MESSAGE(FATAL_ERROR "When using the Apple's Clang compiler, Clang's version must have to be 11.0.0 or greater.")
  ENDIF ()

  # When using LLVM binutils, version must have to be 9.0 or greater
  IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM")
    EXECUTE_PROCESS(COMMAND "${CMAKE_OBJCOPY}" "--version" OUTPUT_VARIABLE OBJCOPY_VERSION)
    STRING(REGEX MATCH "[0-9]+\\.[0-9]+[^\n\r\t\ ]*" OBJCOPY_VERSION "${OBJCOPY_VERSION}")
    IF (OBJCOPY_VERSION VERSION_LESS 9.0)
      MESSAGE(FATAL_ERROR "When using the LLVM binutils, LLVM's version must have to be 9.0 or greater.")
    ENDIF ()
  ENDIF ()

  # When using GCC, version must have to be 9.0 or greater
  IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
      AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    MESSAGE(FATAL_ERROR "When using the GCC compiler, GCC's version must have to be 9.0 or greater.")
  ENDIF ()

  # When using GNU binutils, version must have to be 2.33 or greater
  IF ("${KERNEL_BINUTILS}" STREQUAL "GNU")
    EXECUTE_PROCESS(COMMAND "${CMAKE_OBJCOPY}" "--version" OUTPUT_VARIABLE OBJCOPY_VERSION)
    STRING(REGEX MATCH "[0-9]+\\.[0-9]+[^\n\r\t\ ]*" OBJCOPY_VERSION "${OBJCOPY_VERSION}")
    IF (OBJCOPY_VERSION VERSION_LESS 2.33)
      MESSAGE(FATAL_ERROR "When using the GNU binutils, binutils's version must have to be 2.33 or greater.")
    ENDIF ()
  ENDIF ()

  # Enable LTO only when using GNU+GNU or LLVM+CLANG
  IF (("${KERNEL_BINUTILS}" STREQUAL "${CMAKE_CXX_COMPILER_ID}")
       OR
  ("${KERNEL_BINUTILS}" STREQUAL "LLVM" AND
  ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")))
    SET(LTO_AVAILABLE TRUE)
  ENDIF ()

  IF (CMAKE_BUILD_TYPE)
    SET_AND_EXPORT_FORCE("KERNEL_BUILD_${CMAKE_BUILD_TYPE}" ON BOOL ON
                         "This variable is intended to be used in the CMake Header file.")
  ENDIF ()

ENDIF ()
