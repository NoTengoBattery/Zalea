//===-- asan.c - Address Sanitizer Support ----------------------------------------------------------------*- C -*-===//
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
//
// SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// Contains the code to support the Address Sanitizer, which can be useful for instrumenting memory access.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <CompilerMagic/CompilerMagic.h>

/// \brief GCC's Address Sanitizer API.
///
/// GCC (any GNU compatible compiler) will call this function to store *long* when the Address Sanitizer is enabled.
/// \todo When you done with memory allocation and error reporting, implement ASAN.
/// \param store (more info is required)
void __asan_store1_noabort(unsigned long store) {
    if (store != 0) {
        return;
    }
}
