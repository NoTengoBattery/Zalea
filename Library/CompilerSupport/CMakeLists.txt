#===-- CMakeLists.txt - Compiler Library ---------------------------------------------------------------*- CMake -*-===#
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
#/ This contains the basic compiler support. This contains a series of headers, imported by the CompilerMagic, which
#/ defines some macros to deal with compiler peculiarities.
#/
#/ This module include:
#/ -> CompilerMagic
#/ -> CompilerRuntime
#/ -> ExceptionHandler
#/ -> RuntimeTypeInformation
#/ -> StackUnwinder
#/
#===----------------------------------------------------------------------------------------------------------------===#

SET(THIS_INTERFACE_NAME CompilerMagic)
ADD_LIBRARY("${THIS_INTERFACE_NAME}" INTERFACE)
TARGET_INCLUDE_DIRECTORIES("${THIS_INTERFACE_NAME}" INTERFACE "CompilerMagic")
TARGET_LINK_LIBRARIES("${THIS_INTERFACE_NAME}" INTERFACE "ImportantOutputDirectory")
TARGET_SOURCES("${THIS_INTERFACE_NAME}" INTERFACE "CompilerMagic/AssemblerMagic.h")
TARGET_SOURCES("${THIS_INTERFACE_NAME}" INTERFACE "CompilerMagic/CompilerMagic.h")
TARGET_SOURCES("${THIS_INTERFACE_NAME}" INTERFACE "CompilerMagic/BitwiseMacros.h")
