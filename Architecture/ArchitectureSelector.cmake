#===-- ArchitectureSelector.cmake - Architecture Selector  --------------------------------------------*- CMake -*-===//
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
#/ This file is used to select an architecture from the available ones. This is important because is the root of the
#/ whole configuration process. As from the architecture you can select your machine, compiler and only then you can
#/ configure the kernel features.
#/
#/ The other module is Machine Selector. This module selects the machine from the available machines and includes it's
#/ default config. This "default config" is populated to the cache before selecting the compiler
#/
#===---------------------------------------------------------------------------------------------------------------===//

# List all subdirectories, they are the available architectures
LIST_SUBDIR(${TREE_ARCHITECTURE_PATH} AVAILABLE_ARCHS)
IF (NOT AVAILABLE_ARCHS)
  MESSAGE(FATAL_ERROR
          "No architectures to build! You shouldn't have to read this message ever!")
ELSE ()
  CLIST_TO_HLIST(AVAILABLE_ARCHS HVALID_ARCH)
  MESSAGE(STATUS "Found the following architectures: ${HVALID_ARCH}")
ENDIF ()

# Create a new cache variable, append these architectures to their available values and check if valid
SET_WITH_STRINGS(KERNEL_ARCH "" "Target architecture for building this kernel" AVAILABLE_ARCHS)
CHECK_WITH_STRINGS(KERNEL_ARCH VALID_ARCH)
IF (NOT VALID_ARCH)
  MESSAGE(FATAL_ERROR
          "Please set a valid architecture in the KERNEL_ARCH variable. Available architectures: ${HVALID_ARCH}")
ENDIF ()
