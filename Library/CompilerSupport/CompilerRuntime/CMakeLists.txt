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
# optimizer about the standard library. For example, code that the compiler finds that is a "memcpy" will be optimized
# to a memcpy call. Therefore, we provide those calls here.
ADD_LIBRARY("CompilerRuntime" STATIC
            "phony.c")
IF (KERNEL_ARM)
  TARGET_SOURCES("CompilerRuntime" PRIVATE
                 "${KERNEL_ARCH}/${KERNEL_ARCH}Utils.h")
ELSEIF (KERNEL_x86)
  TARGET_SOURCES("CompilerRuntime" PRIVATE
                 "${KERNEL_ARCH}/${KERNEL_ARCH}Utils.h")
ENDIF ()
TARGET_INCLUDE_DIRECTORIES("CompilerRuntime" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
# This accessory library is the "libgcc" part. It does contains calls made by the compiler when the CPU or the platform
# does not support an operation (such as ARM not supporting integer division). It contains the sanitizers, the stack
# check protector and so on. It *does* require special compiler flags to disable fancy features (such as sanitizing the
# sanitizer itself), and the backend support (i.e. the error reporting of the sanitizer) also requires all of these
# features to be disabled.
#
# TODO: This means that the code that supports those features needs to have it's PRIVATE_COMPILE_OPTIONS set, so this
# TODO: target will compile against those targets with all features disabled. Some features to disable are:
# TODO: lto, sanitizers, rtti, unwind, exceptions, stack protector
#
# TODO: All libraries that need this kind of modifications should be named as their analog but beginning with "Raw"
# TODO: All "Raw" libraries will be dependencies of their analog, therefore, you only need to link the "non-raw"
# TODO: version.
ADD_LIBRARY("RawCompilerRuntime" STATIC
            "asan.c"
            "division.c")
TARGET_DISABLE_FANCY_FEATURES("RawCompilerRuntime" PRIVATE C FANCY_LIST LTO SANITIZER STACK_PROTECTOR)

TARGET_LINK_LIBRARIES("RawCompilerRuntime" "CoreLibraries")
TARGET_LINK_LIBRARIES("CompilerRuntime" "CoreLibraries" "RawCompilerRuntime")
