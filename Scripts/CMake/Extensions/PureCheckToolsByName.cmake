#===-- PureCheckToolsByName.cmake - Check Tools By Name  ----------------------------------------------*- CMake -*-===//
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
#/ Includes some a special ad-hoc macro that tries to check the compiler/binutils by name from a list of supported
#/ compiler/binutils that are set inside the main architecture file.
#/
#/ This extension provides:
#/ -> CHECK_TOOL_BY_NAMES
#/
#===---------------------------------------------------------------------------------------------------------------===//

# This macro will check a tool by it's name. The tool will have it's name in the KERNEL_${TOOL} variable in the cache.
#  The list of available options for the tool must be in the AVAILABLE_${TOOL} variable, as it's the same for
#  SET_WITH_STRINGS.
# A special variable CMAKE_${TOOL}_PATH will be exposed in the cache and it will be a hint for CMake to find such tool.
MACRO(CHECK_TOOL_BY_NAME TOOL DEFAULT)
  SET_WITH_STRINGS("KERNEL_${TOOL}" "${DEFAULT}"
                   "Select the ${TOOL} that will be used to build this kernel"
                   "AVAILABLE_${TOOL}")
  CHECK_WITH_STRINGS("KERNEL_${TOOL}" "VALID_${TOOL}")
  IF (NOT "${VALID_${TOOL}}")
    CLIST_TO_HLIST("AVAILABLE_${TOOL}" "H_AVAILABLE_${TOOL}")
    MESSAGE(FATAL_ERROR "Please set a valid ${TOOL} in the KERNEL_${TOOL} variable. Available options for ${TOOL}: "
            "${H_AVAILABLE_${TOOL}}")
  ENDIF ()

  SET(CMAKE_${TOOL}_PATH "" CACHE FILEPATH "A hint path to use when searching for the ${TOOL}.")
ENDMACRO()
