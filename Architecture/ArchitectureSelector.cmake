#===-- ArchitectureSelector.cmake - Architecture Selector ----------------------------------------------*- CMake -*-===#
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
#/ This file is used to select an architecture from the available ones. The architecture files will setup the supported
#/ tools and the available machines.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# List all subdirectories, they are the available architectures
LIST_SUBDIR("${TREE_ARCHITECTURE_PATH}" AVAILABLE_ARCHITECTURES)
IF (NOT AVAILABLE_ARCHITECTURES)
  MESSAGE(FATAL_ERROR
          "No architectures to build! You shouldn't have to read this message ever!")
ELSE ()
  CMAKE_LIST_TO_HUMAN_LIST(AVAILABLE_ARCHITECTURES H_VALID_ARCH)
  MESSAGE(STATUS "Found the following architectures: ${H_VALID_ARCH}.")
ENDIF ()

# Create a new cache variable, append these architectures to their available values and check if valid
SET_WITH_STRINGS(KERNEL_ARCH "" "Target architecture for building this kernel." AVAILABLE_ARCHITECTURES)
CHECK_WITH_STRINGS(KERNEL_ARCH VALID_ARCH)
IF (NOT VALID_ARCH)
  MESSAGE(FATAL_ERROR "Please set a valid architecture in the KERNEL_ARCH variable. Available architectures: "
          "${H_VALID_ARCH}.")
ENDIF ()

# Update the source tree variables (because they are needed by the MachineSelector script)
MESSAGE(STATUS "Valid architecture selected: \"${KERNEL_ARCH}\". Updating the source tree definition...")
INCLUDE("${TREE_SELF_PATH}")

# Export the current architecture to the default configuration
SET_AND_EXPORT_FORCE(KERNEL_ARCH "${KERNEL_ARCH}" STRING "${KERNEL_ARCH}"
                     "This variable is the architecture to build, which is the CPU architecture that the machine runs.")

# This will export a variable to the config.h file which can be used by C/C++ to enable architecture specific code
SET_AND_EXPORT("KERNEL_${KERNEL_ARCH}" ON INTERNAL ON "-")
