#===-- SetAndExport.cmake - SET_AND_EXPORT Extension ---------------------------------------------------*- CMake -*-===#
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
#/ This is an extension file. This file implements the SetAndExport extension providing command to set a variable to the
#/ cache, and exporting the variable to a database; also dumping the database to a file that can be loaded and used to
#/ populate the cache on a first run and a header to be used as a configuration header for C/C++.
#/
#/ This extension provides:
#/ -> DATABASE_TO_CMAKE
#/ -> DATABASE_TO_HEADER
#/ -> SET_AND_EXPORT
#/ -> SET_AND_EXPORT_FORCE
#/
#===----------------------------------------------------------------------------------------------------------------===#

MESSAGE(STATUS "Importing the SET_AND_EXPORT CMake extension...")

# Initialize the module
IF (NOT SAE_INITDB)
 MESSAGE(STATUS "Initializing module for SET_AND_EXPORT command")
 IF (NOT SET_AND_EXPORT_DATABASE OR
     NOT SET_AND_EXPORT_OUTPUT_FILE OR
     NOT SET_AND_EXPORT_TEMPLATE_FILE OR
     NOT SET_AND_EXPORT_PYTHON_HELPER)
  MESSAGE(FATAL_ERROR "To use the SET_AND_EXPORT extension you must set these variables:\n"
          "SET_AND_EXPORT_DATABASE: Path to the database file,\n"
          "SET_AND_EXPORT_OUTPUT_FILE: Path to generate the CMake config,\n"
          "SET_AND_EXPORT_TEMPLATE_FILE: Path to a file to dump at the beginning of SET_AND_EXPORT_OUTPUT_FILE,\n"
          "SET_AND_EXPORT_PYTHON_HELPER: Path to the Python helper script")
 ENDIF ()
 SET(SET_AND_EXPORT_DATABASE "${SET_AND_EXPORT_DATABASE}" CACHE INTERNAL "Path to the SET_AND_EXPORT database file.")
 SET(SET_AND_EXPORT_OUTPUT_FILE "${SET_AND_EXPORT_OUTPUT_FILE}" CACHE INTERNAL "SET_AND_EXPORT output file.")
 SET(SET_AND_EXPORT_TEMPLATE_FILE "${SET_AND_EXPORT_TEMPLATE_FILE}" CACHE INTERNAL "SET_AND_EXPORT template file.")
 SET(SET_AND_EXPORT_PYTHON_HELPER "${SET_AND_EXPORT_PYTHON_HELPER}" CACHE INTERNAL "SET_AND_EXPORT helper script.")
 # Run the Python script to initialize the DB
 SET(CMD_ARGS
     "INT"
     "--dbfile" "${SET_AND_EXPORT_DATABASE}")
 RUN_PYTHON3_SCRIPT("${SET_AND_EXPORT_PYTHON_HELPER}" "." "${CMD_ARGS}")
 SET(SAE_INITDB ON CACHE INTERNAL "SET_AND_EXPORT database initialized status")
 MESSAGE(STATUS "Module for SET_AND_EXPORT command initialized!")
ENDIF ()

# Dumps the database to a CMake script full of SET_AND_EXPORT and SET_AND_EXPORT_FORCE directives.
FUNCTION( DATABASE_TO_CMAKE )
 SET(CMD_ARGS
     "END"
     "--dbfile" "${SET_AND_EXPORT_DATABASE}"
     "--template" "${SET_AND_EXPORT_TEMPLATE_FILE}"
     "--file" "${SET_AND_EXPORT_OUTPUT_FILE}")
 RUN_PYTHON3_SCRIPT("${SET_AND_EXPORT_PYTHON_HELPER}" "." "${CMD_ARGS}")
 MESSAGE(STATUS "The current configuration is stored in "
         "'${SET_AND_EXPORT_OUTPUT_FILE}'")
ENDFUNCTION()

# Dumps the database to a header script full of #cmakedefine directives.
FUNCTION( DATABASE_TO_HEADER )
 SET(CMD_ARGS
     "END"
     "--header"
     "--dbfile" "${SET_AND_EXPORT_DATABASE}"
     "--template" "${SET_AND_EXPORT_TEMPLATE_HEADER}"
     "--file" "${SET_AND_EXPORT_OUTPUT_HEADER}")
 RUN_PYTHON3_SCRIPT("${SET_AND_EXPORT_PYTHON_HELPER}" "." "${CMD_ARGS}")
 MESSAGE(STATUS "The current 'config.h' header is stored in "
         "'${SET_AND_EXPORT_OUTPUT_HEADER}'")
ENDFUNCTION()

# Sets a variable in the cache and uses a Python3 Script (helper script) to write the variable, type, value and
# docstring to a database. The command that updates the database always use the current value of the variable, no matter
# how the function was called.
FUNCTION( SET_AND_EXPORT VARIABLE VALUE TYPE DEFAULT DOCSTRING )
 # We run "SET" first as it is supposed to not change the cached value (that is the one that we export to the DB)
 # WE NEVER EXPORT THE VALUE GIVEN IN THE FUNCTION CALL, we always export the cached value
 IF (NOT VALUE)
  SET(VALUE "${DEFAULT}")
 ENDIF ()
 SET("${VARIABLE}" "${VALUE}" CACHE "${TYPE}" "${DOCSTRING}")
 # Run the Python script to insert in the DB
 SET(CMD_ARGS
     "SAE"
     "--dbfile" "${SET_AND_EXPORT_DATABASE}"
     "--variable" "${VARIABLE}"
     "--value" "$CACHE{${VARIABLE}}"
     "--type" "${TYPE}"
     "--default" "${DEFAULT}"
     "--docstring" "${DOCSTRING}")
 RUN_PYTHON3_SCRIPT("${SET_AND_EXPORT_PYTHON_HELPER}" "." "${CMD_ARGS}")
ENDFUNCTION()

# Forces a variable in the cache and uses a Python3 Script (helper script) to write the variable, type, value and
# docstring to a database. The command that updates the database always use the provided value in the function call, no
# matter what value the variable currently have.
# SET_AND_EXPORT_FORCE is designed to override a user-selectable config that may render the built kernel useless if
# the user sets the variable in the CMake cache. You should use this directive only in manually modified Default
# Configs or in variables that are constants but need to be exported.
FUNCTION( SET_AND_EXPORT_FORCE VARIABLE VALUE TYPE DEFAULT DOCSTRING )
 # We run "SET" first as it is supposed to change the cached value (that is the one that we won't export to the DB)
 # WE ALWAYS EXPORT THE CACHED VALUE, it should have changed
 IF (NOT VALUE)
  SET(VALUE "${DEFAULT}")
 ENDIF ()
 SET("${VARIABLE}" "${VALUE}" CACHE "${TYPE}" "${DOCSTRING}" FORCE)
 # Run the Python script to insert in the DB
 SET(CMD_ARGS
     "SAE"
     "--dbfile" "${SET_AND_EXPORT_DATABASE}"
     "--variable" "${VARIABLE}"
     "--value" "$CACHE{${VARIABLE}}"
     "--type" "${TYPE}"
     "--default" "${DEFAULT}"
     "--docstring" "${DOCSTRING}"
     "--force")
 RUN_PYTHON3_SCRIPT("${SET_AND_EXPORT_PYTHON_HELPER}" "." "${CMD_ARGS}")
ENDFUNCTION()
