#===-- CMakeLists.txt - Rules to Build the Main Executable ---------------------------------------------*- CMake -*-===#
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
#/ This file will define the CMake rules to build the Main Executable. Some rules are generic, which are listed here.
#/ Some other rules are architecture-specific, which are included from the Architecture subtree.
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This library defines some headers useful to determine the layout and the structure of the final image
ADD_LIBRARY("ExecutableLibrary" INTERFACE)
TARGET_SOURCES("ExecutableLibrary" INTERFACE
               "ExecutableLibrary/ImageConstants.h")
TARGET_INCLUDE_DIRECTORIES("ExecutableLibrary" INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
# This is the final target, the monolithic main executable.
SET(THIS_EXE_NAME LinkedImage)
ADD_EXECUTABLE("${THIS_EXE_NAME}"
               "Multiboot2.cxx"
               "Multiboot2.hxx")
TARGET_LINK_LIBRARIES("${THIS_EXE_NAME}"
                      "CoreLibraries"
                      "EntryPoint"
                      "NonStdCLibrary"
                      "NonStdC++Library")
# Add the linker script to the linker command line (requires linker and compiler detection)
IF (KERNEL_COMPILER_GNU AND KERNEL_LINKER_GNU)
 TARGET_LINK_OPTIONS("${THIS_EXE_NAME}" BEFORE
                     PRIVATE "-Wl,-T${TREE_BIN_IMPORTANT_PATH}/${THIS_EXE_NAME}.ld"
                     PRIVATE "-Wl,-Map=${TREE_BIN_IMPORTANT_PATH}/${THIS_EXE_NAME}.map")
ELSE ()
 MESSAGE(AUTHOR_WARNING "Could not setup the linker script. The generated binaty will not work.")
ENDIF ()
# Add the custom command which will run the C preprocessor to generate a linker script (requires compiler detection)
IF (KERNEL_COMPILER_GNU AND NOT KERNEL_COMPILER_ICC)
 ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" PRE_LINK VERBATIM
                    COMMAND "${CMAKE_C_COMPILER}"
                    ARGS "-E" "-C" "-CC" "-D__LD__" "-P" "-Wall" "-Werror" "-x" "c"
                    "-I" "${TREE_BIN_IMPORTANT_INCLUDE_PATH}"
                    "-I" "${CMAKE_CURRENT_LIST_DIR}"
                    "-o" "${TREE_BIN_IMPORTANT_PATH}/${THIS_EXE_NAME}.ld"
                    "${TREE_EXECUTABLES_PATH}/LinkerLibrary/${KERNEL_ARCH}/MainExecutable.in.ld")
ELSEIF (KERNEL_COMPILER_ICC)
 ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" PRE_LINK VERBATIM
                    COMMAND "${CMAKE_C_COMPILER}"
                    ARGS "-E" "-C" "-D__LD__" "-P" "-Wall" "-Werror" "-x" "c"
                    "-I" "${TREE_BIN_IMPORTANT_INCLUDE_PATH}"
                    "-I" "${CMAKE_CURRENT_LIST_DIR}"
                    "-o" "${TREE_BIN_IMPORTANT_PATH}/${THIS_EXE_NAME}.ld"
                    "${TREE_EXECUTABLES_PATH}/LinkerLibrary/${KERNEL_ARCH}/MainExecutable.in.ld")
ELSE ()
 MESSAGE(AUTHOR_WARNING
         "Could not preprocess the linker script. This means that you are using an incompatible compiler.")
ENDIF ()
# Add the custom command which will dump the contents of the linked image to a file (requires binutils detection)
IF (KERNEL_BINUTILS_GNU)
 ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" POST_BUILD VERBATIM
                    COMMAND "${CMAKE_OBJDUMP}"
                    ARGS "-S" "-d" "-x"
                    "$<TARGET_FILE:${THIS_EXE_NAME}>" ">" "$<TARGET_FILE:${THIS_EXE_NAME}>.objdump"
                    COMMENT "Generating the object dump (objdump) for ${THIS_EXE_NAME}...")
 ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" POST_BUILD VERBATIM
                    COMMAND "${CMAKE_NM}"
                    ARGS "--special-syms" "-C" "-S" "-f" "sysv" "-n"
                    "$<TARGET_FILE:${THIS_EXE_NAME}>" ">" "$<TARGET_FILE:${THIS_EXE_NAME}>.nm"
                    COMMENT "Generating the symbol dump (nm) for ${THIS_EXE_NAME}...")
 ADD_CUSTOM_COMMAND(TARGET "${THIS_EXE_NAME}" POST_BUILD VERBATIM
                    COMMAND "${CMAKE_READELF}"
                    ARGS "-a" "-e" "-g" "-n" "-u"
                    "$<TARGET_FILE:${THIS_EXE_NAME}>" ">" "$<TARGET_FILE:${THIS_EXE_NAME}>.readelf"
                    COMMENT "Generating the ELF dump (readelf) for ${THIS_EXE_NAME}...")
ELSE ()
 MESSAGE(AUTHOR_WARNING
         "Could not setup the object dump rule. This means that you are using incompatible binutils.")
ENDIF ()
# Add the custom command which will strip the binary and generate the raw binary (requires binutils detection)
IF (KERNEL_BINUTILS_GNU)
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
                    ARGS "-S" "-d" "-x"
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
                    COMMENT "Stripping the binary and generating the raw binary for the Main Executable...")
ELSE ()
 MESSAGE(FATAL_ERROR
         "Could not setup the binary post processing rule. This means that you are using incompatible binutils.")
ENDIF ()
