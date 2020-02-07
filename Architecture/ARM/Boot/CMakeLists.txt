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
#===----------------------------------------------------------------------------------------------------------------===#

ADD_LIBRARY("EntryPoint" STATIC
            "EntryPoint.c"
            "EntryPoint.sx"
            "${MACHINE_MARCH}.vectors.sx")
TARGET_LINK_LIBRARIES("EntryPoint" "CoreLibraries")
TARGET_DISABLE_FANCY_FEATURES("EntryPoint" PRIVATE C FANCY_LIST LTO SANITIZER STACK_PROTECTOR)
