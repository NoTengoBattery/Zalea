#===-- PureTargetUtils.cmake - Utilities for Managing CMake Targets ------------------------------------*- CMake -*-===#
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
#
# SPDX-License-Identifier: Apache-2.0
#
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ This file contains several functions and macros for managing target attributes, flags, properties and more.
#/
#/ This extension provides:
#/ -> TARGET_DISABLE_FANCY_FEATURES
#/
#===----------------------------------------------------------------------------------------------------------------===#

# This macro will grab a target name in it's first argument, the visibility for the options, the linkage language and a
# variable (which is a list) and will disable all the requested "fancy features" for that target. This works by
# appending flags to the target which disables the previously enabled features. Not all features can be disabled and the
# exact flag is defined in the Platform files.
MACRO( TARGET_DISABLE_FANCY_FEATURES TARGET VISIBILITY LINKAGE FANCY_LIST )
 IF (NOT ${ARGV3} STREQUAL FANCY_LIST)
  MESSAGE(FATAL_ERROR
          "You should call this macro with the target name, the options visibility, the linkage, the word "
          "'FANCY_LIST' followed by the feature list to be disabled.")
 ENDIF ()
 FOREACH (FEATURE ${ARGN})
  IF (${LINKAGE}_NO_${FEATURE})
   TARGET_COMPILE_OPTIONS("${TARGET}" "${VISIBILITY}" "${${LINKAGE}_NO_${FEATURE}}")
  ENDIF ()
 ENDFOREACH ()
ENDMACRO()
