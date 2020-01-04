#===-- PureVarsUtils.cmake - Variables Utils  ---------------------------------------------------------*- CMake -*-===//
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
# SPDX-License-Identifier: Apache-2.0
#
#===---------------------------------------------------------------------------------------------------------------===//
#/
#/ \file
#/ Contains some utilities and macros for handling variables.
#/
#/ This extension provides:
#/ -> CHECK_WITH_STRINGS
#/ -> CLIST_TO_HLIST
#/ -> SET_WITH_STRINGS
#/
#===---------------------------------------------------------------------------------------------------------------===//

# Checks the current value of a variable VARIABLE against the list of strings in it's STRINGS property.
FUNCTION(CHECK_WITH_STRINGS VARIABLE RESULT)
  SET(VAR_VAL ${${VARIABLE}})
  GET_PROPERTY(STR_LIST CACHE ${VARIABLE} PROPERTY STRINGS)
  IF ("${STR_LIST}" STREQUAL "")
    MESSAGE(WARNING "Variable in the cache has no property STRINGS!")
    SET(${RESULT} OFF PARENT_SCOPE)
  ELSE ()
    IF ("${VAR_VAL}" STREQUAL "")
      SET(${RESULT} OFF PARENT_SCOPE)
    ELSE ()
      IF ("${VAR_VAL}" IN_LIST STR_LIST)
        SET(${RESULT} ON PARENT_SCOPE)
      ELSE ()
        SET(${RESULT} OFF PARENT_SCOPE)
      ENDIF ()
    ENDIF ()
  ENDIF ()
ENDFUNCTION()

# Converts a CMake list to a comma separated human list
MACRO(CLIST_TO_HLIST VARIABLE RESULT)
  STRING(REPLACE ";" ", " ${RESULT} "${${VARIABLE}}")
ENDMACRO()

# Sets a variable VARIABLE to the cache with an initial value of INIT_VAL and a docstring DESCRIPTION. The STR_LIST is
# a list of strings that populates the STRINGS property of the VARIABLE.
MACRO(SET_WITH_STRINGS VARIABLE INIT_VAL DESCRIPTION STR_LIST)
  SET(${VARIABLE} ${INIT_VAL} CACHE STRING ${DESCRIPTION})
  SET_PROPERTY(CACHE ${VARIABLE} PROPERTY STRINGS ${${STR_LIST}})
ENDMACRO()
