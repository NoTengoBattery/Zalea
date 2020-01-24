#===-- MainExecutable.cmake - Rules to build the final (main) executable  ------------------------------*- CMake -*-===#
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
#/ This file will define the CMake rules to build the final executable. Some rules are generic, which are listed here.
#/ Some other rules are architecture-specific, which are included from the Architecture subtree.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# The name of the executable image (just after linking)
SET(THIS_EXE_NAME LinkedImage)

# Add the executable to the project
ADD_EXECUTABLE("${THIS_EXE_NAME}")

# Link these "libraries" to the executable. These libraries are the full build tree: they depend on other "libraries"
TARGET_LINK_LIBRARIES("${THIS_EXE_NAME}"
                      EntryPoint)
