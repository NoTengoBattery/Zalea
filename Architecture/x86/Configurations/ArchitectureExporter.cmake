#===-- ArchitectureExporter.cmake - Export Variables Defined by the Configuration  ----*- CMake -*-===#
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
#/ This file will export some variables to the auto-generated files to make them accessible to C/C++/ASM/Linker Scripts
#/ and to the CMake default configuration.
#/
#===----------------------------------------------------------------------------------------------------------------===#

SET_AND_EXPORT(MACHINE_LOAD_ADDRESS "${MACHINE_LOAD_ADDRESS}" STRING "0x00000000"
               "This will set the ELF load address.")
SET_AND_EXPORT(MACHINE_PHYSICAL_ADDRESS "${MACHINE_PHYSICAL_ADDRESS}" STRING "0x00000000"
               "This will set the ELF physical address.")
SET_AND_EXPORT(MACHINE_LOADABLE_IMAGE_NAME "${MACHINE_LOADABLE_IMAGE_NAME}" STRING "KernelImage"
               "This will set the name of the binary that the machine can load. Some machines require a specific file.")
