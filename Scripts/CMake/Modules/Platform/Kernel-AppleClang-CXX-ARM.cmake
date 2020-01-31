#===-- Kernel-Clang-CXX-ARM.cmake - CMake System-Compiler-Language-Architecture File ====---------------*- CMake -*-===#
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
#/ This file contains System-specific, Compiler-specific, Language-specific and Architecture-specific code and
#/ variables. This file is called just after CMake identifies the compiler, and before the Compiler-specific,
#/ Language-specific configuration file when CMake checks for a "working compiler".
#/
#===----------------------------------------------------------------------------------------------------------------===#

IF (TREE_SELF_PATH) # This will define if we have access to the scope variables and cache

  SET(CMAKE_CXX_FLAGS_INIT)
  SET(CMAKE_CXX_FLAGS_DEBUG_INIT)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL_INIT)
  SET(CMAKE_CXX_FLAGS_RELEASE_INIT)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT)
  SET(CMAKE_EXE_LINKER_FLAGS_INIT)

  # Those are special base flags which will setup the ABI for the compiler
  STRING(APPEND CMAKE_CXX_FLAGS_INIT "-mabi=aapcs ")
  STRING(APPEND CMAKE_CXX_FLAGS_INIT "-mfloat-abi=soft ")
  STRING(APPEND CMAKE_CXX_FLAGS_INIT "-mtp=soft ")

  # These flags are based on which kind of build we are doing
  STRING(APPEND CMAKE_CXX_FLAGS_DEBUG_INIT " ")
  STRING(APPEND CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-mthumb ")
  STRING(APPEND CMAKE_CXX_FLAGS_RELEASE_INIT "-mthumb ")
  STRING(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT " ")

ENDIF ()
