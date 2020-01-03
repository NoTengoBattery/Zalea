#===-- CMakeToolchain.cmake - The root of the toolchain file for CMake cross compilation  -------------*- CMake -*-===//
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
#
#===---------------------------------------------------------------------------------------------------------------===//
#/
#/ \file
#/ This file is the root of the toolchain architecture for CMake. This file will evaluate and configure the toolchain as
#/ it's detected. It will import some files which are ad-hoc for each toolchain.
#/
#===---------------------------------------------------------------------------------------------------------------===//

