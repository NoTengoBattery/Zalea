//===-- BasesMagic.h - Provide some numeric constants that represents bases -----------------------------*- C++ -*-===//
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
//  SPDX-License-Identifier: Apache-2.0
//
//===--------------------------------------------------------------------------------------------------------------===//
///
/// \file
/// This file can be imported inside any other target, and used inside any source file where numeric and base constant
/// directives are needed.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_BASESMAGIC_H
#define ZALEA_BASESMAGIC_H

#ifndef __ASSEMBLER__  // C and C++

enum constants {
 Zero = 0,
 Zeroth = Zero,
 One = 1,
 First = One,
 Two = 2,
 Second = Two,
 Three = 3,
 Third = Three,
 Four = 4,
 Fourth = Four,
 Five = 5,
 Fifth = Five,
 Six = 6,
 Sixth = Six,
 Seven = 7,
 Seventh = Seven,
 Eight = 8,
 Eighth = Eight,
 Nine = 9,
 Ninth = Nine,
 Ten = 10,
 Tenth = Ten,
 Eleven = 11,
 Eleventh = Eleven,
 Twelve = 12,
 Twelfth = Twelve,
 Thirteen = 13,
 Thirteenth = Thirteen,
 Fourteen = 14,
 Fourteenth = Fourteen,
 Fifteen = 15,
 Fifteenth = Fifteen,
 Sixteen = 16,
 Sixteenth = Sixteen
};

enum bases {
 OctalBase = Eight,
 DecimalBase = Ten,
 HexadecimalBase = Sixteen
};

#ifdef __cplusplus  // C++
#endif
#else  // ASM
#endif

#endif //ZALEA_BASESMAGIC_H
