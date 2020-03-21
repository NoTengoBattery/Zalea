//===-- memcpy.c - An Implementation of the `memcpy` C Standard Function ----------------------------------*- C -*-===//
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
/// This is an implementation of `memcpy` for the Non Standard C Library. The compiler, some times, will detect a
/// `memcpy`-like behaviour and will replace the code with a `memcpy` call. This does provide the standard C API for
/// `memcpy`.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include "string.h"

void *memcpy(void *destination, const void *source, size_t length) {
 extern void *__memcpy(void *, const void *, size_t);
 __memcpy(destination, source, length);  // Use the implementation inside the Compiler Runtime
 return destination;
}
