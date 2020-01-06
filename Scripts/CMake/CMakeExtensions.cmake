#===-- CMakeExtensions.cmake - CMake Language Extensions  ---------------------------------------------*- CMake -*-===//
#
# Copyright (c) 2019 Oever González
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
#/ Extensions for the CMake language. These extensions may be in "pure CMake" (because they come from a file that
#/ begins with "Pure") or they may use helper Python scripts.
#/
#/ These extensions provide an API which is intended to be permanent and reliable. This file will import extension
#/ scripts one by one. For new extensions to be added, you may have to add them to a extension script or create a new
#/ extension script and modify this file by hand.
#/
#===---------------------------------------------------------------------------------------------------------------===//

MESSAGE(STATUS "CMake Language Extensions for \"${PROJECT_NAME}\"")

# Include all extensions, ordered alphabetically, one by one.
INCLUDE("${TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH}/PureFileUtils.cmake")
INCLUDE("${TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH}/PurePythonRunner.cmake")
INCLUDE("${TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH}/PureVarsUtils.cmake")
INCLUDE("${TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH}/SetAndExport.cmake")
