//===-- ImageConstants.h - Link-Time Constant Addresses of the Binary Image -------------------------------*- C -*-===//
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
/// All of these constants are pointers that live inside the final binary image. They are constants at link time.
///
//===--------------------------------------------------------------------------------------------------------------===//

#ifndef ZALEA_IMAGECONSTANTS_H
#define ZALEA_IMAGECONSTANTS_H

extern volatile void *bssEnd;
extern volatile void *bssStart;
extern volatile void *imageEnd;
extern volatile void *imageStart;

extern volatile void *multibootStructPointer;

extern void miserableFail(void);

#endif //ZALEA_IMAGECONSTANTS_H
