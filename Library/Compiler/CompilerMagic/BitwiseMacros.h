//===-- BitwiseMacros.h - Macros to Use Bitwise Operations ------------------------------------------------*- C -*-===//
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
/// This file can be imported inside any other target, and used inside any source file where compiler-specific
/// directives are needed. This file will attempt to
///
//===--------------------------------------------------------------------------------------------------------------===//

#include <config.h>
#include <CompilerMagic.h>

#ifndef ZALEA_BITWISE_MACROS_H
#define ZALEA_BITWISE_MACROS_H

#define SET_UNSIGNED_NBIT(x, y) (x ## U | (1U << y ## U))
#define CLEAR_UNSIGNED_NBIT(x, y) (x ## U & ~(1U << y ## U))

#endif //ZALEA_BITWISE_MACROS_H
