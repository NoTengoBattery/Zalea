#===-- Tree.cmake - Project's source tree description  ---*- CMake -*-===//
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
#/ Contains many variables exported to CMake describing the source tree structure. These variables are relative to the
#/ root CMakeLists.txt and shall be used whenever a reference to a in-source path is needed. The variables are, however
#/ a full path.
#/
#/ Variable names starts with TREE, following with the relative path to the folder they represent in snake case, all
#/ caps. They always end with '_PATH'.
#/
#/ In order to list a subfolder, the parent's path variable is used and not the raw path. Subfolders are listed in
#/ order after it's parent folder. This is for making refactoring easier: only track the folder's name and not the
#/ folder's path.
#/
#/ All variables in this script contains parts that are hardcoded! This is intentional: you can rely in these variables.
#/
#===---------------------------------------------------------------------------------------------------------------===//

# Point to this file itself
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "The tree definition is setting up a pointer to itself")
  SET(TREE_SELF_PATH ${CMAKE_CURRENT_LIST_FILE})
ENDIF ()

# Root folders
SET(TREE_SRC_ROOT_PATH ${CMAKE_SOURCE_DIR})                                                                  # <- Parent
MESSAGE(STATUS "Source root path: ${TREE_SRC_ROOT_PATH}")
SET(TREE_BIN_ROOT_PATH ${CMAKE_BINARY_DIR})                                                                  # <- Parent
MESSAGE(STATUS "Binary/build root path: ${TREE_BIN_ROOT_PATH}")

# Output important root folder
SET(TREE_BIN_IMPORTANT_PATH ${CMAKE_BINARY_DIR}/Important)                                                   # <- Parent
MESSAGE(STATUS "Build important files root path: ${TREE_BIN_IMPORTANT_PATH}")

# Scripts root folder
SET(TREE_SCRIPTS_PATH ${TREE_SRC_ROOT_PATH}/Scripts)                                                         # <- Parent
MESSAGE(STATUS "Scripts root path: ${TREE_SCRIPTS_PATH}")
SET(TREE_SCRIPTS_CMAKE_PATH ${TREE_SCRIPTS_PATH}/CMake)
SET(TREE_SCRIPTS_CMAKE_MODULES_PATH ${TREE_SCRIPTS_CMAKE_PATH}/Modules)
SET(TREE_SCRIPTS_CMAKE_TOOLCHAIN_PATH ${TREE_SCRIPTS_CMAKE_PATH}/Toolchain)
