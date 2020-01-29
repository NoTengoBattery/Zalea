//===-- CPlusPlusMainExecutable.cxx - Source File for the Main Executable -------------------------------*- C++ -*-===//
//
// Copyright (c) 2020 Oever González
//
//  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
//  the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
//  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
//  specific language governing permissions and limitations under the License.
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This is a phony file. This file will tell CMake to use the C++ linker for the final executable. This file is empty
/// and dead code. This file is common to all architectures and targets and it's sole purpose is to be a placeholder.
///
//===--------------------------------------------------------------------------------------------------------------===//

extern "C" {
extern void start();
extern void *multiBootHeader;
}

void notMain();

void notMain() {
    start();
}
