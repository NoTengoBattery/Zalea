#===-- CMakeToolchainCompiler.cmake - The compiler-side file for CMake cross compilation  -------------*- CMake -*-===//
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
#/ This file is the compiler-side of the toolchain architecture for CMake. This file will evaluate and configure the
#/ compiler as it's detected.
#/
#===---------------------------------------------------------------------------------------------------------------===//

# Try to use the binary installed by the user in non-standard PATH, which is set in CMAKE_COMPILER_PATH
FIND_PROGRAM(CMAKE_ASM_EXE NAMES "${CMAKE_ASM_COMPILER}" HINTS "${CMAKE_COMPILER_PATH}" PATH_SUFFIXES "bin"
             DOC "This is a guess made by the build system for an assembly compiler")
FIND_PROGRAM(CMAKE_C_EXE NAMES "${CMAKE_C_COMPILER}" HINTS "${CMAKE_COMPILER_PATH}" PATH_SUFFIXES "bin"
             DOC "This is a guess made by the build system for an C compiler")
FIND_PROGRAM(CMAKE_CXX_EXE NAMES "${CMAKE_CXX_COMPILER}" HINTS "${CMAKE_COMPILER_PATH}" PATH_SUFFIXES "bin"
             DOC "This is a guess made by the build system for an C++ compiler")

# Mark these 3 variables as advanced (not to be shown in the "normal" GUI
MARK_AS_ADVANCED(CMAKE_ASM_EXE)
MARK_AS_ADVANCED(CMAKE_C_EXE)
MARK_AS_ADVANCED(CMAKE_CXX_EXE)

# Set the actual CMake variables that will point to the right compiler
SET(CMAKE_ASM_COMPILER "${CMAKE_ASM_EXE}")
SET(CMAKE_C_COMPILER "${CMAKE_C_EXE}")
SET(CMAKE_CXX_COMPILER "${CMAKE_CXX_EXE}")
