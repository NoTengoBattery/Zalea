#===-- CMakeLists.txt - Non Standard C++ Library -------------------------------------------------------*- CMake -*-===#
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
#/ This is the Non Standard C++ Library, which gives support to the C++ dialect used by this project.
#/
#===----------------------------------------------------------------------------------------------------------------===#

ADD_LIBRARY("NonStdC++Library" STATIC
            "cstdint"
            "phony.cxx")
TARGET_INCLUDE_DIRECTORIES("NonStdC++Library" INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
TARGET_LINK_LIBRARIES("NonStdC++Library" "CoreLibraries" "NonStdCLibrary")
