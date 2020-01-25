#===-- CMakeLists.txt - Rules to build the final (main) executable  ------------------------------------*- CMake -*-===#
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
ADD_EXECUTABLE("${THIS_EXE_NAME}"
               "CPlusPlusMainExecutable.cxx"
               "LinkerScriptDefinitions.h"
               "MainExecutableBase.h"
               "${CONFIGURATION_HEADER}")

# Link these "libraries" to the executable. These libraries are the full build tree: they depend on other "libraries"
TARGET_LINK_LIBRARIES("${THIS_EXE_NAME}"
                      EntryPoint)

# Add the linker script to the linker command line (requires binutils and compiler detection)
IF (("${KERNEL_BINUTILS}" STREQUAL "LLVM"
     OR "${KERNEL_BINUTILS}" STREQUAL "GNU")
     AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
     OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
     OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"))
  TARGET_LINK_LIBRARIES("${THIS_EXE_NAME}" "-Wl,-T\"${TREE_BIN_IMPORTANT_PATH}/MainExecutable.ld\""
                        "-Wl,-Map=\"${TREE_BIN_IMPORTANT_PATH}/MainExecutable.map\"")
ENDIF ()

# Add the custom command which will run the C++ preprocessor to generate a linker script (requires compiler detection)
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"
    OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"
    OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" PRE_LINK VERBATIM
                     COMMAND "${CMAKE_CXX_COMPILER}"
                     ARGS "-x" "assembler-with-cpp" "-P"
                     "-I" "${TREE_BIN_IMPORTANT_INCLUDE_PATH}"
                     "-I" "${CMAKE_CURRENT_LIST_DIR}"
                     "-o" "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.ld"
                     "-E" "${TREE_EXECUTABLES_PATH}/${KERNEL_ARCH}/MainExecutable.in.ld")
ENDIF ()

# Add the custom command which will dump the contents of the linked image to a file (requires binutils detection)
IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM"
    OR "${KERNEL_BINUTILS}" STREQUAL "GNU")
  ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" POST_BUILD VERBATIM
                     COMMAND "${CMAKE_OBJDUMP}"
                     ARGS "-x" "-d" "-s" "-S"
                     "$<TARGET_FILE:${THIS_EXE_NAME}>"
                     ">" "$<TARGET_FILE:${THIS_EXE_NAME}>.objdump"
                     COMMENT "Generating the object dump for the main executable...")
ENDIF ()

# Add the custom command which will strip the binary and generate the raw binary (requires binutils detection)
IF ("${KERNEL_BINUTILS}" STREQUAL "LLVM"
    OR "${KERNEL_BINUTILS}" STREQUAL "GNU")
  ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" POST_BUILD VERBATIM
                     COMMAND "${CMAKE_OBJCOPY}"
                     ARGS "--only-keep-debug"
                     "$<TARGET_FILE:${THIS_EXE_NAME}>"
                     "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.dbg"
                     COMMAND "${CMAKE_OBJCOPY}"
                     ARGS "--strip-all"
                     "$<TARGET_FILE:${THIS_EXE_NAME}>"
                     "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.elf"
                     COMMAND "${CMAKE_OBJCOPY}"
                     ARGS "--add-gnu-debuglink=${TREE_BIN_IMPORTANT_PATH}/MainExecutable.dbg"
                     "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.elf"
                     COMMAND "${CMAKE_OBJDUMP}"
                     ARGS "-x" "-d" "-s" "-S"
                     "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.elf"
                     ">" "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.objdump"
                     COMMAND "${CMAKE_OBJCOPY}"
                     ARGS "-O" "binary"
                     "$<TARGET_FILE:${THIS_EXE_NAME}>"
                     "${TREE_BIN_IMPORTANT_PATH}/MainExecutable.bin"
                     COMMAND "${CMAKE_OBJCOPY}"
                     ARGS "-O" "binary"
                     "$<TARGET_FILE:${THIS_EXE_NAME}>"
                     "${TREE_BIN_IMPORTANT_PATH}/${MACHINE_LOADABLE_IMAGE_NAME}"
                     COMMENT "Stripping the binary and generating the raw binary for the main executable...")
ENDIF ()
