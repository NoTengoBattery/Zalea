#===-- CMakeLists.txt - Root of the ARM/Boot Subtree --------------------------------------------------*- CMake -*-===#
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
#/ This file will include the entry point for the Main Executable for ARM. The Main Executable contains all the code
#/ that will make the device fully functional.
#/
#/ TODO: In the future, we probably will compress the Main Executable image and append a "header" that will uncompress
#/ TODO: it, and put it in the VMA (by enabling the MMU).
#/
#===----------------------------------------------------------------------------------------------------------------===#

SET(THIS_LIB_NAME "EntryPoint")
ADD_LIBRARY("${THIS_LIB_NAME}" OBJECT
            "EntryPoint.c"
            "EntryPoint.sx")
TARGET_LINK_LIBRARIES("${THIS_LIB_NAME}" "${GLOBAL_INTERFACES}")
