#===-- GenerateDeviceDescriptor.cmake - GENERATE_DEVICE_DESCRIPTOR Extension ---------------------------*- CMake -*-===#
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
#/ This is an extension file. This file implements the HashTable generation from a JSON file. The HashTable is a C array
#/ which can be built in a C project and be accessed by calculating the hashes and by searching near the generated index
#/ value.
#/
#/ This extension provides:
#/ -> GENERATE_DEVICE_DESCRIPTOR
#/
#===----------------------------------------------------------------------------------------------------------------===#

MESSAGE(STATUS "Importing the GENERATE_DEVICE_DESCRIPTOR CMake extension...")

# Initialize the module
IF (NOT JPI_INIT)
 MESSAGE(STATUS "Initializing module for SET_AND_EXPORT command")
 IF (NOT DEVICE_DESCRIPTOR_DATABASE OR
     NOT DEVICE_DESCRIPTOR_HEADER_TEMPLATE OR
     NOT DEVICE_DESCRIPTOR_HEADER OR
     NOT DEVICE_DESCRIPTOR_SOURCE_TEMPLATE OR
     NOT DEVICE_DESCRIPTOR_SOURCE)
  MESSAGE(FATAL_ERROR "To use the GENERATE_DEVICE_DESCRIPTOR extension you must set these variables:\n"
          "DEVICE_DESCRIPTOR_DATABASE: the 'database' that will be converted to the final Device Descriptor\n"
          "DEVICE_DESCRIPTOR_HEADER_TEMPLATE: a template to be included at the beginning of the generated header\n"
          "DEVICE_DESCRIPTOR_HEADER: the path to the generated header file\n"
          "DEVICE_DESCRIPTOR_SOURCE_TEMPLATE: a template to be included at the beginning of the generated source\n"
          "DEVICE_DESCRIPTOR_SOURCE: the path to the generated source file\n")
 ENDIF ()
 SET(JPI_INIT ON CACHE INTERNAL "GENERATE_DEVICE_DESCRIPTOR initialized status")
ENDIF ()

SET_AND_EXPORT(DEVICE_DESCRIPTOR_HASH_BITS "8U" STRING "8U"
               "The number of bits to be used to generate and calculate the hash table for the JSON properties.")
SET_AND_EXPORT(DEVICE_DESCRIPTOR_HASH_FORESEE "2U" STRING "2U"
               "The number of positions to seek around the calculated hash in the event of collision.")

# Compile a JSON properties file in a C source code file and a header that can be used as a Hash Table to access
# properties that may be undiscoverable, but are not wise or useful to embed in the source code itself. This way,
# undiscoverable properties can be accessed in a "modular" fashion.
FUNCTION( GENERATE_DEVICE_DESCRIPTOR )
 SET(CMD_ARGS
     "--yaml-file" "${DEVICE_DESCRIPTOR_DATABASE}"
     "--header-template" "${DEVICE_DESCRIPTOR_HEADER_TEMPLATE}"
     "--header" "${DEVICE_DESCRIPTOR_HEADER}"
     "--source-template" "${DEVICE_DESCRIPTOR_SOURCE_TEMPLATE}"
     "--source" "${DEVICE_DESCRIPTOR_SOURCE}"
     "--bits" "${DEVICE_DESCRIPTOR_HASH_BITS}"
     "--foresee" "${DEVICE_DESCRIPTOR_HASH_FORESEE}"
     "--api-struct-name" "deviceProperty"
     "--api-table-name" "deviceDescriptor")
 RUN_PYTHON3_SCRIPT("${DEVICE_DESCRIPTOR_PYTHON_HELPER}" "${TREE_DEVICE_DESCRIPTOR_PATH}" "${CMD_ARGS}")
 MESSAGE(STATUS "Generated Device Descriptor header file in '${DEVICE_DESCRIPTOR_HEADER}'")
 MESSAGE(STATUS "Generated Device Descriptor source file in '${DEVICE_DESCRIPTOR_SOURCE}'")
ENDFUNCTION()
