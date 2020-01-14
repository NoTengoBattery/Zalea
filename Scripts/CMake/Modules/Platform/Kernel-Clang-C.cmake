#===-- Kernel-Clang-C.cmake - CMake System-Compiler-Language file ====---------------------------------*- CMake -*-===//
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
#/ just after CMake identifies the compiler, and before the Compiler-specific, Language-specific configuration file when
#/ CMake checks for a "working compiler".
#/
#===---------------------------------------------------------------------------------------------------------------===//

# Those are the base "freestanding" flags
SET(CMAKE_C_FLAGS "'--target=${KERNEL_TARGET}' '-ffreestanding' '-nostdlib'")
# Those are the base "machine" and "architecture" flags
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} '-march=${MACHINE_MARCH}' '-mtune=${MACHINE_MTUNE}'")
# This flag defines the linker to be used
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} '-fuse-ld=${CMAKE_LD}'")
# If the user gave the system a path for binutils, tell the driver to use it first
IF (CMAKE_BINUTILS_BIN_PATH)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} '-B${CMAKE_BINUTILS_BIN_PATH}'")
ENDIF ()
