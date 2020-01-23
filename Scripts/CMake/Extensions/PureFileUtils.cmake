#===-- PureFileUtils.cmake - File Utils  ---------------------------------------------------------------*- CMake -*-===#
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
#/ Includes some utilities for handling, discovering and listing files and directories.
#/
#/ This extension provides:
#/ -> LIST_FILE_FILTER
#/ -> LIST_SUBDIR
#/
#===----------------------------------------------------------------------------------------------------------------===#

# List all the files in the directory DIR whose name matches with the EXPR. Stores the result in the variable provided
# in RESULT. When REMOVE_MATCHER is set, all file names will be returned with a simple regex replace from EXPR to an
# empty string.
MACRO(LIST_FILE_FILTER DIR EXPR REMOVE_MATCHER RESULT)
  FILE(GLOB FILES RELATIVE "${DIR}" "${DIR}/*")
  SET(FILE_LIST "")
  FOREACH (FILE ${FILES})
    IF (NOT IS_DIRECTORY "${DIR}/${FILE}")
      STRING(REGEX MATCH "${EXPR}" MATCHES "${FILE}")
      IF (MATCHES)
        IF (${REMOVE_MATCHER})
          STRING(REGEX REPLACE "${EXPR}" "" FILE "${FILE}")
        ENDIF ()
        LIST(APPEND FILE_LIST "${FILE}")
      ENDIF ()
    ENDIF ()
  ENDFOREACH ()
  LIST(SORT FILE_LIST)
  SET(${RESULT} ${FILE_LIST})
ENDMACRO()

# List all the subdirectories in the given directory
MACRO(LIST_SUBDIR DIR RESULT)
  FILE(GLOB FILES RELATIVE "${DIR}" "${DIR}/*")
  SET(FILE_LIST "")
  FOREACH (FILE ${FILES})
    IF (IS_DIRECTORY "${DIR}/${FILE}")
      LIST(APPEND FILE_LIST "${FILE}")
    ENDIF ()
  ENDFOREACH ()
  LIST(SORT FILE_LIST)
  SET(${RESULT} ${FILE_LIST})
ENDMACRO()
