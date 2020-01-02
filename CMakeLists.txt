#===-- CMakeLists.txt - Root of the CMake project  ----------------------------------------------------*- CMake -*-===//
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
#/ This is the main CMake file, the root of the CMake project. From this file the whole project is generated. The
#/ functionality of this file is delegated among the whole tree of imported files and the files that they import,
#/ therefore, this is a minimal file to get the project building and working.
#/
#===---------------------------------------------------------------------------------------------------------------===//

# The following line sets the minimum CMake version required to build this project
CMAKE_MINIMUM_REQUIRED(VERSION 3.15.3)

# We won't support in-tree build, however, we support build in a tree subdirectory
IF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  MESSAGE(FATAL_ERROR "In-source builds are not supported. Please run CMake in an different directory.")
ENDIF ()

# Include tree definition. The 'Tree.cmake' file sets the tree structure, all the needed paths
INCLUDE(Tree.cmake)
# Always generate these important directories if they don't exist
FILE(MAKE_DIRECTORY ${TREE_BIN_IMPORTANT_PATH})
# Set the TREE_SCRIPTS_CMAKE_MODULES_PATH to the CMAKE_MODULE_PATH so CMake will use those modules first
SET(CMAKE_MODULE_PATH "${TREE_SCRIPTS_CMAKE_MODULES_PATH}")
SET(CMAKE_TOOLCHAIN_FILE ${TREE_SCRIPTS_CMAKE_TOOLCHAIN_PATH}/CMakeToolchain.cmake)

# We rely on Python 3.7 for CMake Extensions, which are Python scripts that extends our CMake
SET(Python_ROOT_DIR "${TREE_SCRIPTS_PYTHON_ENVS_PATH}")
FIND_PACKAGE(Python3 COMPONENTS Interpreter)
IF (NOT Python3_Interpreter_FOUND)
  MESSAGE(FATAL_ERROR "No Pyhton 3 interpreter found. Python 3.7 or later is required.")
ELSEIF (Python3_VERSION VERSION_LESS 3.7)
  MESSAGE(FATAL_ERROR "Python 3.7 or newer is required to build this project.")
ELSE ()
  MESSAGE(STATUS "Found a suitable version of Python 3: \"Python ${Python3_VERSION}\"")
ENDIF ()

# Include "Language Extensions" (those need Python)
INCLUDE(${TREE_SCRIPTS_CMAKE_PATH}/CMakeExtensions.cmake)

# For a CMake cross-compilation (the one used throughout this process), you must specify some variables that will
# disable compilation for the host and enable the use of Platform, Compiler and Language files. Along with the
# toolchain files, it is possible to create a fully customized compilation system based on CMake.
# -> CMAKE_SYSTEM_NAME is set to Kernel
# -> CMAKE_SYSTEM_VERSION is set to a particular version, shared between the image and the loader
# -> CMAKE_CROSSCOMPILING to ON
# -> CMAKE_SYSTEM_PROCESSOR is set in a temp cache variable from another CMake file and then set here
SET(CMAKE_SYSTEM_NAME Kernel)
SET(CMAKE_SYSTEM_VERSION 0.0.0.1)
SET(CMAKE_CROSSCOMPILING ON)
SET(CMAKE_SYSTEM_PROCESSOR ${KERNEL_ARCH})

# Create and configure the actual CMake project
GET_FILENAME_COMPONENT(KERNEL_SOURCEDIR_NAME "${CMAKE_SOURCE_DIR}" NAME_WE)
MESSAGE(STATUS "Using the directory name as the project identifier: \"${KERNEL_SOURCEDIR_NAME}\"")
# Enable the lenguages: C, C++, CPP and Assembler
PROJECT("Zalea ${CMAKE_SYSTEM_NAME} (${KERNEL_SOURCEDIR_NAME})"
        VERSION ${CMAKE_SYSTEM_VERSION} LANGUAGES ASM C CXX)#CPP)
MESSAGE(STATUS "The \"${PROJECT_NAME}\" project version ${CMAKE_PROJECT_VERSION} for ${KERNEL_ARCH}")
