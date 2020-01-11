#===-- CheckBinutilsName.cmake - Compiler Name Checker  ---------------------------------------*- CMake -*-===//
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

SET_WITH_STRINGS(KERNEL_BINUTILS "GNU" "Select the binutils that will be used to build this kernel" AVAILABLE_BINUTILS)
CHECK_WITH_STRINGS(KERNEL_BINUTILS VALID_BINUTILS)
IF (NOT VALID_BINUTILS)
  CLIST_TO_HLIST(AVAILABLE_BINUTILS H_AVAILABLE_BINUTILS)
  MESSAGE(FATAL_ERROR "Please set a valid binutils in the KERNEL_BINUTILS variable. Available binutils: "
          "${H_AVAILABLE_BINUTILS}")
ENDIF ()

SET(CMAKE_BINUTILS_PATH "" CACHE FILEPATH "A hint path to use when searching for the binutils.")
