#===-- MachineSelector.cmake - Machine Selector  -------------------------------------------------------*- CMake -*-===#
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
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ This file list all available default configuration files in the architecture directory, exposes them to the cache and
#/ forces the user to select a machine.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# List all configuration files in a subdirectory
#  Those files have a special extension .cfg.cmake which will make them available to the cache
LIST_FILE_FILTER("${TREE_ARCHITECTURE_X_CONFIG_PATH}" "\\.cfg\\.cmake$" ON AVAILABLE_MACHINES)
IF (NOT AVAILABLE_MACHINES)
  MESSAGE(AUTHOR_WARNING "Implementing a new architecture? You can create an empty temporary configuration file. The "
          "current configuration will be in the Important directory. This current configuration has the same format as "
          "the default settings. When the current configuration is ready, you can move or copy the to create an "
          "authentic default configuration.")
  MESSAGE(FATAL_ERROR "No machine configurations found in '${TREE_ARCHITECTURE_X_CONFIG_PATH}'")
ENDIF ()

# Create a new cache variable, append these architectures to their available values and check if valid
SET_WITH_STRINGS(KERNEL_MACHINE "${KERNEL_MACHINE}" "Target machine for building this kernel." AVAILABLE_MACHINES)
CHECK_WITH_STRINGS(KERNEL_MACHINE VALID_MACHINE)
IF (NOT VALID_MACHINE)
  IF (NOT KERNEL_MACHINE)
    MESSAGE(FATAL_ERROR "You must pick a valid machine configuration! All available configurations are listed in the "
            "KERNEL_MACHINE-STRINGS variable in the cache. They are, also, listed in the ccmake GUI and they are the "
            "files in the '${TREE_ARCHITECTURE_X_CONFIG_PATH}' directory (without the extension).")
  ELSE ()
    MESSAGE(FATAL_ERROR "The machine picked does not have a '.cfg.cmake' configuration file in "
            "'${TREE_ARCHITECTURE_X_CONFIG_PATH}'. You can create an empty file for a new machine and update it with "
            "the exported CMake config in the Important path.")
  ENDIF ()
ELSE ()
  IF (NOT _MACHINE_INCLUDED)
    INCLUDE("${TREE_ARCHITECTURE_X_CONFIG_PATH}/${KERNEL_MACHINE}.cfg.cmake")
    SET(_MACHINE_INCLUDED "YES" CACHE INTERNAL "Switch to avoid overriding user's configuration.")
  ENDIF ()
  MESSAGE(STATUS "Building the \"${MACHINE_NAME}\" machine...")
ENDIF ()

# Export the current architecture to the default configuration
SET_AND_EXPORT(MACHINE_NAME "${MACHINE_NAME}" STRING " "
               "This variable is the machine for the build, a human readable name for the machine.")

# This file will give the options for valid compilers (including compiler, assembler, linker and it's minimum versions)
INCLUDE("${TREE_ARCHITECTURE_PATH}/${KERNEL_ARCH}/${KERNEL_ARCH}.cmake")
