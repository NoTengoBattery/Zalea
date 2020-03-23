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
            "memcpy.c"
            "memset.c"
            "strcmp.c"
            "strlen.c"
            "strtoullC.c"
            "strtoullC.h")
TARGET_INCLUDE_DIRECTORIES("CompilerRuntime" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
TARGET_CHANGE_FEATURES("CompilerRuntime" PRIVATE C FEATURES NO_STACK_PROTECTOR)
ADD_LIBRARY("RawCompilerRuntime" STATIC
            "division.c"
            "division.h"
            "${KERNEL_ARCH}/${KERNEL_ARCH}Utils.h")
IF (KERNEL_ARM)
 TARGET_SOURCES("RawCompilerRuntime" PRIVATE
                "${KERNEL_ARCH}/__aeabi_uidivmod.c"
                "${KERNEL_ARCH}/__aeabi_uldivmod.c"
                "${KERNEL_ARCH}/arm_noop.sx")
ENDIF ()
TARGET_INCLUDE_DIRECTORIES("RawCompilerRuntime" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
TARGET_CHANGE_FEATURES("RawCompilerRuntime" PRIVATE C FEATURES OPTIMIZE_MAXIMUM NO_LTO NO_STACK_PROTECTOR)

TARGET_LINK_LIBRARIES("RawCompilerRuntime" "CoreLibraries")
TARGET_LINK_LIBRARIES("CompilerRuntime" "RawCompilerRuntime" "CoreLibraries")
