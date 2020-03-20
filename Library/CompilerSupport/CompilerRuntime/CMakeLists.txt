#===-- CMakeLists.txt - Compiler Runtime Library -------------------------------------------------------*- CMake -*-===#
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
#/ This contains the core of the compiler support. It provides the runtime assumptions and library calls made by the
#/ compiler.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This is the compiler runtime, but not the "libgcc" (or "compiler-rt") part, it is the assumptions done by the
# optimizer about the standard library. For example, code that the compiler finds that is a `memcpy` will be optimized
# to a `memcpy` call. Therefore, we implement these functions here but provide them on the Non Standard C Library.
ADD_LIBRARY("CompilerRuntime" STATIC
            "division.c"
            "division.h"
            "memcpy.c"
            "memset.c"
            "strcmp.c"
            "strlen.c"
            "${KERNEL_ARCH}/${KERNEL_ARCH}Utils.h")
TARGET_INCLUDE_DIRECTORIES("CompilerRuntime" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
TARGET_DISABLE_FANCY_FEATURES("CompilerRuntime" PRIVATE C FANCY_LIST LTO_ONLY STACK_PROTECTOR)
# This accessory library is the "libgcc" part. It does contains calls made by the compiler when the CPU or the platform
# does not support an operation (such as ARM not supporting integer division). It contains the sanitizers, the stack
# check protector and so on. It *does* require special compiler flags to disable fancy features (like avoiding to
# sanitize the sanitizer itself), and the backend support. Note that LTO will generate code that might change these
# flags.
#
# TODO: That means that for LTO, you will need to carefully enable or disable the features to ensure the minimum area
# TODO: where such features are disabled.
#
# TODO: For non-LTO, the code generated will be always generated without the flags and linked as-is, therefore, you will
# TODO: not have to disable these features in the software that use the non-LTO (raw machine code) compiled versions.
ADD_LIBRARY("RawCompilerRuntime" STATIC)
IF (KERNEL_ARM)
  TARGET_SOURCES("RawCompilerRuntime" PRIVATE
                 "${KERNEL_ARCH}/__aeabi_uidivmod.c")
ENDIF ()
SET_TARGET_PROPERTIES("RawCompilerRuntime" PROPERTIES LINKER_LANGUAGE C)
TARGET_INCLUDE_DIRECTORIES("RawCompilerRuntime" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
TARGET_DISABLE_FANCY_FEATURES("RawCompilerRuntime" PRIVATE C FANCY_LIST LTO STACK_PROTECTOR)

TARGET_LINK_LIBRARIES("RawCompilerRuntime" "CompilerRuntime" "CoreLibraries")
TARGET_LINK_LIBRARIES("CompilerRuntime" "CoreLibraries" "RawCompilerRuntime")
