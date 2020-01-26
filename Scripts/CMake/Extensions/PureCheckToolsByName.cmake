#===-- PureCheckToolsByName.cmake - Check Tools By Name  -----------------------------------------------*- CMake -*-===#
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
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ Includes some a special ad-hoc macro that tries to check the compiler/binutils by name from a list of supported
#/ compiler/binutils that are set inside the main architecture file.
#/
#/ This extension provides:
#/ -> CHECK_TOOL_BY_NAME
#/ -> FORCE_TOOL_BY_NAME
#/ -> GUESS_TOOL_BY_NAME
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This macro will check a tool by it's name. The tool will have it's name in the KERNEL_${TOOL} variable in the cache.
#  The list of available options for the tool must be in the AVAILABLE_${TOOL} variable, as it's the same for
#  SET_WITH_STRINGS.
# A special variable CMAKE_${TOOL}_PATH will be exposed in the cache and it will be a hint for CMake to find such tool.
MACRO(CHECK_TOOL_BY_NAME TOOL DEFAULT)
  SET_WITH_STRINGS("KERNEL_${TOOL}" "${DEFAULT}"
                   "Select the ${TOOL} that will be used to build this project."
                   "AVAILABLE_${TOOL}")
  CHECK_WITH_STRINGS("KERNEL_${TOOL}" "VALID_${TOOL}")
  IF (NOT "${VALID_${TOOL}}")
    CLIST_TO_HLIST("AVAILABLE_${TOOL}" "H_AVAILABLE_${TOOL}")
    MESSAGE(FATAL_ERROR "Please set a valid ${TOOL} in the KERNEL_${TOOL} variable. Available options for ${TOOL}: "
            "${H_AVAILABLE_${TOOL}}")
  ENDIF ()

  SET(CMAKE_${TOOL}_PATH "" CACHE FILEPATH "A hint path to use when searching for the ${TOOL}.")
ENDMACRO()

# This macro will force the guess of a tool executable by searching the user's provided PATH first.
MACRO(FORCE_TOOL_BY_NAME TYPE TOOL)
  UNSET("CMAKE_${TYPE}" CACHE)
  UNSET("CMAKE_${TYPE}_EXE" CACHE)
  FIND_PROGRAM("CMAKE_${TYPE}_EXE" NAMES "${CMAKE_${TYPE}}" HINTS "${CMAKE_${TOOL}_PATH}" PATH_SUFFIXES "bin" ""
               DOC "This is a guess forced by the build system for a tool.")
  SET("CMAKE_${TYPE}" "${CMAKE_${TYPE}_EXE}")
  SET("CMAKE_${TYPE}" "${CMAKE_${TYPE}_EXE}" CACHE FILEPATH "A path to a tool used by CMake.")
  MARK_AS_ADVANCED("CMAKE_${TYPE}")
  MARK_AS_ADVANCED("CMAKE_${TYPE}_EXE")
  IF (NOT CMAKE_${TYPE})
    MESSAGE(FATAL_ERROR
            "The CMake build system could not find the 'CMAKE_${TYPE}' tool. The configuration process will stop now. "
            "You can give the CMake build system a hint by setting up the cache 'CMAKE_${TOOL}_PATH' variable to the "
            "path where the tool can be found, or by fixing your 'PATH' environment variable.")
  ENDIF ()
ENDMACRO()

# This macro will try to guess a tool executable by searching the user's provided PATH first, then letting CMake
# fallback to a default compiler and failing later (if the compiler does not work).
MACRO(GUESS_TOOL_BY_NAME TYPE TOOL)
  IF (NOT CMAKE_${TYPE}_EXE)
    UNSET("CMAKE_${TYPE}" CACHE)
    UNSET("CMAKE_${TYPE}_EXE" CACHE)
    FIND_PROGRAM("CMAKE_${TYPE}_EXE" NAMES "${CMAKE_${TYPE}}" HINTS "${CMAKE_${TOOL}_PATH}" PATH_SUFFIXES "bin" ""
                 DOC "This is a guess made by the build system for a tool.")
  ENDIF ()
  SET("CMAKE_${TYPE}" "${CMAKE_${TYPE}_EXE}")
  SET("CMAKE_${TYPE}" "${CMAKE_${TYPE}_EXE}" CACHE FILEPATH "A path to a tool used by CMake.")
  MARK_AS_ADVANCED("CMAKE_${TYPE}")
  MARK_AS_ADVANCED("CMAKE_${TYPE}_EXE")
ENDMACRO()
