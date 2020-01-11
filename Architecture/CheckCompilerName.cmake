#===-- CheckCompilerName.cmake - Compiler Name Checker  ---------------------------------------*- CMake -*-===//
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
#/ This file is the responsible of checking and enforcing a valid compiler from the list that is known to build this
#/ kernel for this architecture.
#/
#===---------------------------------------------------------------------------------------------------------------===//

SET_WITH_STRINGS(KERNEL_COMPILER "GCC" "Select the compiler that will be used to build this kernel" AVAILABLE_COMPILERS)
CHECK_WITH_STRINGS(KERNEL_COMPILER VALID_COMPILER)
IF (NOT VALID_COMPILER)
  CLIST_TO_HLIST(AVAILABLE_COMPILERS H_AVAILABLE_COMPILERS)
  MESSAGE(FATAL_ERROR "Please set a valid compiler in the KERNEL_COMPILER variable. Available compilers: "
          "${H_AVAILABLE_COMPILERS}")
ENDIF ()

SET(CMAKE_COMPILER_PATH "" CACHE FILEPATH "A hint path to use when searching for the compiler.")
# TODO: Shall we implement a macro?
