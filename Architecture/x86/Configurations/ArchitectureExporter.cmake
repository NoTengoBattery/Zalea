#===-- ArchitectureExporter.cmake - Export Variables Defined by the Configuration Files ----------------*- CMake -*-===#
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
#/ This file will export some variables to the auto-generated files to make them accessible to C/C++/ASM/Linker Scripts
#/ and to the CMake default configuration. All of these variables are "MACHINE" variables, they only exist here to be
#/ exported, to not overload the other files with machine-specific SET_AND_EXPORT.
#/
#===----------------------------------------------------------------------------------------------------------------===#

SET_AND_EXPORT(MACHINE_LOAD_ADDRESS "${MACHINE_LOAD_ADDRESS}" STRING "0x00000000"
               "This is the load address of the image, used by the loader to place the image in memory.")
SET_AND_EXPORT(MACHINE_LOADABLE_IMAGE_NAME "${MACHINE_LOADABLE_IMAGE_NAME}" STRING "KernelImage"
               "This will set the name of the binary that the machine can run. Some machines require a specific file "
               "name.")
SET_AND_EXPORT(MACHINE_STACK_DOWNWARDS ON BOOL ON
               "If this value is true, the machine's stack will grow downwards. Otherwise it will grow upward.")
SET_AND_EXPORT(MACHINE_STACK_SIZE "${MACHINE_STACK_SIZE}" STRING "0x4000"
               "This is the stack size for the machine. It's default is 0x4000, which is 16 KiB.")
SET_AND_EXPORT(MACHINE_VIRTUAL_ADDRESS "${MACHINE_VIRTUAL_ADDRESS}" STRING "0x00000000"
               "This is the 'virtual' address, which is the one that is referenced by the compiled code.")
