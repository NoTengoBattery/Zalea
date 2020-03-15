//===-- division.h - Divide Integer Numbers ---------------------------------------------------------------*- C -*-===//
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
/// This file tries to implement division of integer numbers. This is probably not fast at all but it's required by
/// targets that does not support integer division such as the ARM (some CPUs do support integer division but the
/// compiler will know). In theory, this code is an implementation of the large binary division.
///
/// This is probably the slowest possible implementation but anyway... it does work good.
///
//===--------------------------------------------------------------------------------------------------------------===//


#ifndef ZALEA_DIVISION_H
#define ZALEA_DIVISION_H

/// If the sign bit flag is enabled, the number is positive.
#define SIGN_FLAG 0x00U

/// If the div0 bit flag is enabled, a division by zero has occurred.
#define DIV_0_FLAG 0x01U

/// \brief This special struct defines a number with flags.
///
/// All numbers in this struct are signed, but the value is unsigned and it's sign is defined by the sign bit in the
/// flags field. A special process is required to build a number from a *normal* number.
struct signedT {
    /// The flags of this number. Flags are defined by the macros in this file.
    unsigned flags;
    /// The unsigned value of this number.
    unsigned value;
};

/// \brief This is a struct that represents a division. It has two fields, which are signedT values.
///
/// Thus, this struct represents a signed division between two integer numbers.
struct divisionT {
    struct signedT numerator;
    struct signedT denominator;
};

/// \brief This is the result of the division operation.
///
/// This struct is designed to return the values of the quotient and remainder after the division, thus returning both
/// results in the same call.
struct resultT {
    /// Remainder is always positive or 0, by definition
    unsigned remainder;
    /// The signedT struct is to recycle the signed division for unsigned values, just discarding the sign.
    struct signedT quotient;
};

/// \brief Perform a long division.
///
/// This function will perform a long signed division between the 2 integers provided and will return the result by
/// changing the values in the struct provided by the caller.
/// \param operands a divisionT that represents the two operands.
/// \param result a pointer to a resultT struct that this function will modify to return the results.
void longDivision(struct divisionT *operands, struct resultT *result);

#endif //ZALEA_DIVISION_H
