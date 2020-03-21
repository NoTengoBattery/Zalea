//===-- strtoullC.c - Implementation of `strtoull` (globally usable function) -----------------------------*- C -*-===//
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
/// Implementation of `strtoull` which is globally usable. This implementation can be used globally, and it's the base
/// of the Non Standard C Library and part of the Compiler Runtime, which means that it is similar to a compiler
/// intrinsic.
///
//===--------------------------------------------------------------------------------------------------------------===//

#include "strtoullC.h"
#include <CompilerMagic/BitwiseMacros.h>
#include <InlineMagic/ArithmeticUtils.h>
#include <InlineMagic/AsciiUtils.h>
#include <stddef.h>

/// \brief a generalized implementation of `strtoull`, which can be used to implement the other (strto... and ato...)
///
/// This function is a customized version. The standard-compatible versions can be used from the Non Standard C Library.
/// The main deference is that this function will take an struct pointer and use the provided struct to return the
/// result. This is because this function can convert an arbitrary string to an unsigned long long value with a sign,
/// which can be used to reinterpret the unsigned value as a signed value, and return it accordingly.
///
/// \note This implementation is inspired in the cplusplus.com/reference/cstdlib/strtol web page.
/// \note This implementation will not accept bases (radix) bigger than 36.
///
/// \param string a pointer to a C-like ASCII string which is to be interpreted.
/// \param endingPointer if not NULL, the pointer's value is changed to the address of the first invalid character.
/// \param base the base to use when performing the conversion.
/// \param result the struct that will be used to return the result of the conversion.
/// \param maximum the maximum value allowed for positive numbers (unsigned)
/// \param minimum the minimum value allowed for positive numbers (unsigned)
void __strtoullC(const char *string, char **endingPointer, const int base,
                 struct strtoullSignedT *result, const uintmax_t maximum, const uintmax_t minimum) {
 // Clear the struct (to avoid problems with random noise).
 result->flags = 0x00;
 result->value = 0x00;
 // Initialize all the constants...
 static const unsigned noBase = 0;
 static const unsigned octalBase = 8;
 static const unsigned decimalBase = 10;
 static const unsigned hexadecimalBase = 16;
 static const unsigned maximumBase = 36;
 static const unsigned minimumBase = 2;
 // Initialize all the variables...
 const char *mobileString = string;
 char character = *mobileString;
 uintmax_t actualLimit = maximum;
 unsigned actualBase = noBase;
 // Discard all the whitespace characters until a non-whitespace character is found.
 while (isAsciiSpace(character)) { character = moveStringPointerOneForward(&mobileString); }
 // The next character is most likely the sign, which can be '-', '+' or none, which then is assumed to be positive.
 switch (character) {
  case '-':result->flags = CLEAR_NTH_BIT(result->flags, SIGN_FLAG);
   actualLimit = minimum;
   character = moveStringPointerOneForward(&mobileString);
   break;
  case '+':result->flags = SET_NTH_BIT(result->flags, SIGN_FLAG);
   character = moveStringPointerOneForward(&mobileString);
   break;
  default:result->flags = SET_NTH_BIT(result->flags, SIGN_FLAG);
   break;
 }
 /*
  * The next character is probably a digit. But we have to be careful, because if the base is 0 or hex, we must accept
  * the "0x" and "0X" prefixes, if the base is 0 or oct, we should accept the prefix "0". If the base is 0 and if no
  * prefix is provided, we should treat the number as base dec. If the base is provided, we treat the number as the
  * provided base.
  */
 if (character == '0') {
  character = moveStringPointerOneForward(&mobileString);
  if ((character == 'x' || character == 'X') &&
    (base == hexadecimalBase || base == noBase)) {
   character = moveStringPointerOneForward(&mobileString);
   actualBase = hexadecimalBase;
  } else if (base == octalBase || base == noBase) { actualBase = octalBase; }
 } else if (base == decimalBase || base == noBase) { actualBase = decimalBase; }
 else { actualBase = base; }
 // If the base is out of range, we return with the BASE flag set.
 if (actualBase > maximumBase || actualBase < minimumBase) {
  result->flags = SET_NTH_BIT(result->flags, BASE_FLAG);
  return;
 }
 /*
  * Perform the actual conversion in this block of code. Since we are using and accepting only ASCII strings (locales
  * not allowed inside a core library), we can safely assume that digit characters from 0 to 9 are contiguous, letters
  * from a to z and from A to Z are contiguous.
  */
 const unsigned char minDecimal = '0';
 const unsigned char maxDecimal = '0' + decimalBase - 0x01;
 const unsigned char maxLowercase = 'a' + actualBase - decimalBase - 0x01;
 const unsigned char minLowercase = 'a';
 const unsigned char maxUppercase = 'A' + actualBase - decimalBase - 0x01;
 const unsigned char minUppercase = 'A';
 uintmax_t accumulator = 0x00;
 while (true) {
  bool overflow = false;
  unsigned computedValue;
  if (character >= minDecimal && character <= maxDecimal) { computedValue = character - minDecimal; }
  else if (character >= minLowercase && character <= maxLowercase) {
   computedValue = character + (decimalBase - minLowercase);
  } else if (character >= minUppercase && character <= maxUppercase) {
   computedValue = character + (decimalBase - minUppercase);
  } else { break; }
  overflow |= safeUnsignedMultiplication(accumulator, actualBase, actualLimit, &accumulator);
  overflow |= safeUnsignedAddition(accumulator, computedValue, actualLimit, &accumulator);
  if (overflow) { result->flags = SET_NTH_BIT(result->flags, RANGE_FLAG); }
  character = moveStringPointerOneForward(&mobileString);
 }
 // Store the value, update the given ending pointer and return.
 result->value = accumulator;
 if (endingPointer != NULL) { *endingPointer = (char *) mobileString; }
}
