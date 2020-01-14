#===--Kernel.cmake - CMake System file  ---------------------------------------------------------------*- CMake -*-===//
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
#/ This file contains System-specific, Compiler-generic and Language-generic code and variables. This file is called
#/ just after CMake identifies the compiler, and before the Compiler-specific, Language-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===---------------------------------------------------------------------------------------------------------------===//

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  IF (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "${KERNEL_COMPILER}")
    IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
      MESSAGE(STATUS "Using Apple's Clang instead of vanilla Clang/LLVM...")
    ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
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

ENDIF ()
