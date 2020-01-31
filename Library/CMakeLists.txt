#===-- CMakeLists.txt - Root of the Library Subtree ---------------------------------------------------*- CMake -*-===#
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
#/ Contains several libraries for the runtime support for the compiler, C and C++. These are not low-level libraries but
#/ rather support libraries. In theory, any low level support must have to be implemented in the machine or platform.
#/
#/ However, these libraries will support the runtime, the compiler, the data structures and the standard library.
#/
#/ This module includes
#/ -> AssemblyLibrary
#/ -> CPlusPlusLibrary
#/ -> NonStdCLibrary
#/ -> CompilerSupport
#/ -> DataStructures
#/
#===----------------------------------------------------------------------------------------------------------------===#

# Add all subdirectories that will be ever used, one by one
ADD_SUBDIRECTORY("${TREE_LIBRARY_COMPILER_PATH}")
