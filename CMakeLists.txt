#===-- CMakeLists.txt - Root of the CMake project ------------------------------------------------------*- CMake -*-===#
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
#/ This is the main CMake file, the root of the CMake project. From this file the whole project is generated. The
#/ functionality of this file is delegated among the whole tree of imported files and the files that they import,
#/ therefore, this is a minimal file to get the project building and working.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# The following line sets the minimum CMake version required to build this project
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

# We won't support in-tree build, however, we support build in a tree subdirectory
IF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  MESSAGE(FATAL_ERROR "In-source builds are not supported. Please run CMake in an different directory.")
ENDIF ()

# Include tree definition. The 'Tree.cmake' file sets the tree structure, all the needed paths and special files
INCLUDE(Tree.cmake)
# Always generate these important directories if they don't exist
FILE(MAKE_DIRECTORY "${TREE_BIN_IMPORTANT_PATH}")
# Set the TREE_SCRIPTS_CMAKE_MODULES_PATH to the CMAKE_MODULE_PATH so CMake will use those modules first
SET(CMAKE_MODULE_PATH "${TREE_SCRIPTS_CMAKE_MODULES_PATH}")

# We rely on Python 3.7 for CMake Extensions, which are Python scripts that extends the build system
FIND_PACKAGE(Python3 COMPONENTS Interpreter)
IF (NOT Python3_Interpreter_FOUND)
  MESSAGE(FATAL_ERROR "No Python 3 interpreter found. Python 3.7 or later is required.")
ELSEIF (Python3_VERSION VERSION_LESS 3.7)
  MESSAGE(FATAL_ERROR "Python 3.7 or newer is required to build this project.")
ELSE ()
  MESSAGE(STATUS "Found a suitable version of Python 3: \"Python ${Python3_VERSION}\"")
  MESSAGE(STATUS "Python executable: '${Python3_EXECUTABLE}'")
ENDIF ()

# Manually include this Python extension. This is the Python runner, a pure CMake runner needed to install the VENV
INCLUDE("${TREE_SCRIPTS_CMAKE_EXTENSIONS_PATH}/PurePythonRunner.cmake")

# Generate the Python Virtual Environment on the first run. Then try to update the CMake paths
IF (NOT PYTHON3_GENERATED)
  MESSAGE(STATUS "Ready to install the Python Virtual Environment...")
  FILE(MAKE_DIRECTORY "${TREE_SCRIPTS_PYTHON3_ENV_PATH}")
  RUN_PYTHON3_SCRIPT("${PYTHON_GENERATE_PY}" "${TREE_SCRIPTS_PYTHON3_ENV_PATH}" ".")
  UNSET(_Python3_EXECUTABLE CACHE)
  FIND_PROGRAM(_Python3_EXECUTABLE NAMES "python3" "python" PATHS
               "${TREE_SCRIPTS_PYTHON3_ENV_PATH}" PATH_SUFFIXES "Scripts" "bin" NO_DEFAULT_PATH)
  SET(Python3_EXECUTABLE "${_Python3_EXECUTABLE}" CACHE INTERNAL "The Python 3 Virtual Environment executable." FORCE)
  SET(_Python3_EXECUTABLE "${_Python3_EXECUTABLE}" CACHE INTERNAL "The Python 3 Virtual Environment executable." FORCE)
  FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)
  SET(PYTHON3_GENERATED "YES" CACHE INTERNAL "Detect if the Python 3 Virtual Environment has been installed.")
ENDIF ()

# For a CMake cross-compilation (the one used throughout this process), you must specify some variables that will
# disable compilation for the host and enable the use of Platform, Compiler and Language files. Along with the
# toolchain files, it is possible to create a fully customized compilation system based on CMake
# -> CMAKE_SYSTEM_NAME is set to Kernel
# -> CMAKE_SYSTEM_VERSION is set to a particular version, shared between the image and the loader
# -> CMAKE_CROSSCOMPILING to ON
# -> CMAKE_SYSTEM_PROCESSOR is set in a temp cache variable from another CMake file and then set here
SET(CMAKE_SYSTEM_NAME Kernel)
SET(CMAKE_SYSTEM_VERSION 0.0.0.1)
SET(CMAKE_CROSSCOMPILING ON)

# Identify the "project name" with the name of the root folder of the source code
GET_FILENAME_COMPONENT(KERNEL_SOURCE_DIR_NAME "${CMAKE_SOURCE_DIR}" NAME_WE)
MESSAGE(STATUS "Using the directory name as the project identifier: \"${KERNEL_SOURCE_DIR_NAME}\"")
SET(PROJECT_NAME "Zalea ${CMAKE_SYSTEM_NAME} (${KERNEL_SOURCE_DIR_NAME})")

# Include "Language Extensions" (need Python), this file includes all available extensions
INCLUDE("${TREE_SCRIPTS_CMAKE_PATH}/CMakeExtensions.cmake")

# This file is the architecture selector, it forces the user to select a valid architecture to build
INCLUDE("${TREE_ARCHITECTURE_PATH}/ArchitectureSelector.cmake")
# This file is the machine selector, it forces the user to select a valid machine to build
INCLUDE("${TREE_ARCHITECTURE_PATH}/MachineSelector.cmake")

# Include the CMake Toolchain file that will force the user to select a compiler for it's target
SET(CMAKE_TOOLCHAIN_FILE "${TREE_SCRIPTS_CMAKE_TOOLCHAIN_PATH}/CMakeToolchain.cmake"
    CACHE INTERNAL "The CMake toolchain file.")

# Update these variables and the tree before the project and after a sane configuration
SET(CMAKE_SYSTEM_PROCESSOR "${KERNEL_ARCH}")
INCLUDE("${TREE_SELF_PATH}")

# Create and configure the actual CMake project and enable the languages: C, C++ and Assembler
PROJECT("${PROJECT_NAME}" VERSION ${CMAKE_SYSTEM_VERSION} LANGUAGES ASM C CXX)
MESSAGE(STATUS "The \"${PROJECT_NAME}\" project version ${CMAKE_PROJECT_VERSION} for ${KERNEL_ARCH}")

# Set the INTERPROCEDURAL_OPTIMIZATION default to true (LTO)
IF (LTO_AVAILABLE)
  SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION YES)
ENDIF ()

# Set the language standard for the project (C11, C++2x)
SET(CMAKE_C_EXTENSIONS OFF)
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED YES)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED YES)

# Add an interface library for config.h (and friends)
ADD_LIBRARY(ImportantOutputDirectory INTERFACE)
TARGET_INCLUDE_DIRECTORIES(ImportantOutputDirectory INTERFACE "${TREE_BIN_IMPORTANT_INCLUDE_PATH}")
TARGET_SOURCES(ImportantOutputDirectory INTERFACE "${CONFIGURATION_HEADER}")
SET(GLOBAL_INTERFACES "CompilerMagic"
    "ImportantOutputDirectory")

# Add all subdirectories (the root subdirectories) that will be ever used
ADD_SUBDIRECTORY("${TREE_ARCHITECTURE_PATH}")
ADD_SUBDIRECTORY("${TREE_LIBRARY_PATH}")

# Dump the Current Config database to a CMake and header file at the end of configuration
DATABASE_TO_CMAKE()
DATABASE_TO_HEADER()

# Configure the CMake Header
CONFIGURE_FILE("${SAE_OUTPUT_HEADER}" "${CONFIGURATION_HEADER}" ESCAPE_QUOTES)

# In this directory we add the main executable and the loader
ADD_SUBDIRECTORY("${TREE_EXECUTABLES_PATH}")
