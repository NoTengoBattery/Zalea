#===-- Tree.cmake - Project's source tree description  ------------------------------------------------*- CMake -*-===//
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

# Root folders
SET(TREE_SRC_ROOT_PATH "${CMAKE_SOURCE_DIR}")                                                                # <- Parent
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "Source root path: ${TREE_SRC_ROOT_PATH}")
ENDIF ()
SET(TREE_BIN_ROOT_PATH "${CMAKE_BINARY_DIR}")                                                                # <- Parent
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "Binary/build root path: ${TREE_BIN_ROOT_PATH}")
ENDIF ()

# Output important root folder
SET(TREE_BIN_IMPORTANT_PATH ${CMAKE_BINARY_DIR}/Important)                                                   # <- Parent
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "Build important files root path: ${TREE_BIN_IMPORTANT_PATH}")
ENDIF ()

# Scripts root folder
SET(TREE_ARCHITECTURE_PATH "${TREE_SRC_ROOT_PATH}/Architecture")                                             # <- Parent
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "Architecture root path: ${TREE_ARCHITECTURE_PATH}")
ENDIF ()
SET(TREE_ARCHITECTURE_X_PATH "${TREE_ARCHITECTURE_PATH}/${KERNEL_ARCH}")
SET(TREE_ARCHITECTURE_X_CONFIG_PATH "${TREE_ARCHITECTURE_X_PATH}/Configurations")
SET(TREE_SCRIPTS_PATH "${TREE_SRC_ROOT_PATH}/Scripts")                                                       # <- Parent
IF (NOT TREE_SELF_PATH)
  MESSAGE(STATUS "Scripts root path: ${TREE_SCRIPTS_PATH}")
ENDIF ()
SET(TREE_SCRIPTS_CMAKE_PATH "${TREE_SCRIPTS_PATH}/CMake")
SET(TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH "${TREE_SCRIPTS_CMAKE_PATH}/Extensions")
SET(TREE_SCRIPTS_CMAKE_MODULES_PATH "${TREE_SCRIPTS_CMAKE_PATH}/Modules")
SET(TREE_SCRIPTS_CMAKE_TOOLCHAIN_PATH "${TREE_SCRIPTS_CMAKE_PATH}/Toolchain")
SET(TREE_SCRIPTS_PYTHON_PATH "${TREE_SCRIPTS_PATH}/Python")
SET(TREE_SCRIPTS_PYTHON_ENV_PATH "${TREE_SCRIPTS_PYTHON_PATH}/Environments")
SET(TREE_SCRIPTS_PYTHON3_ENV_PATH "${TREE_SCRIPTS_PYTHON_PATH}/Environments/3")
SET(TREE_SCRIPTS_PYTHON3_BIN_ENV_PATH "${TREE_SCRIPTS_PYTHON3_ENV_PATH}/bin")

# Important and fixed path files
SET(PYTHON_GENERATE_PY "${TREE_SCRIPTS_PYTHON_ENV_PATH}/generate.py")

# Point to this file itself
SET(TREE_SELF_PATH "${CMAKE_CURRENT_LIST_FILE}")