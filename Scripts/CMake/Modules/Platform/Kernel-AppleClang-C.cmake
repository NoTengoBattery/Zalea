#===-- Kernel-AppleClang-C.cmake - CMake System-Compiler-Language file ====----------------------------*- CMake -*-===//
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
#/ This file contains System-specific, Compiler-specific and Language-specific code and variables. This file is called
#/ just after CMake identifies the compiler, and before the Compiler-specific, Laguage-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===---------------------------------------------------------------------------------------------------------------===//

MESSAGE(FATAL_ERROR "The Apple's version of Clang/LLVM is not supported by this project.\n"
        "In order to use Clang with this project, you will need to set up your environment variable 'PATH' to point "
        "to an official LLVM/Clang build (including a build from source) before any other path in your 'PATH'.\n"
        "See Apple's official help: "
        "https://support.apple.com/guide/terminal/apd382cc5fa-4f58-4449-b20a-41c53c006f8f/mac\n"
        "Alternatively, you can set the CMake cache variable CMAKE_COMPILER_BIN_PATH to the path where the compiler "
        "is installed.\n"
        "See LLVM's releases web page to get a copy of Clang/LLVM: "
        "http://releases.llvm.org/download.html\n"
        "You will need to remove the CMake's cache after setting up your compiler and 'PATH' correctly, or before "
        "setting the CMAKE_COMPILER_BIN_PATH variable with the -D flag, in order to remove this error message.")
