#===-- PurePythonRunner.cmake - Python runner  --------------------------------------------------------*- CMake -*-===//
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
#/ Run a python script with arguments.
#/
#/ This extension provides:
#/ -> RUN_PYTHON_SCRIPT
#/
#===---------------------------------------------------------------------------------------------------------------===//

# Runs a Python script given in the SCRIPT_PATH using the interpreter from a variable Python3_EXECUTABLE which is set
# FIND_PACKAGE. The ARGLIST is a CMake list which holds all arguments.
FUNCTION(RUN_PYTHON3_SCRIPT SCRIPT_PATH WORKING_PATH ARGLIST)
  IF (NOT Python3_EXECUTABLE)
    MESSAGE(FATAL_ERROR "You need a Python executable. Use the FIND_PACKAGE(Python3) before calling this function")
  ENDIF ()
  EXECUTE_PROCESS(
    COMMAND "${Python3_EXECUTABLE}" "${SCRIPT_PATH}" ${ARGLIST}
    WORKING_DIRECTORY ${WORKING_PATH}
    RESULT_VARIABLE PY_RESULT
    OUTPUT_QUIET)
  IF (NOT PY_RESULT EQUAL 0)
    MESSAGE(FATAL_ERROR "Python script returned with error: ${PY_RESULT}")
  ENDIF ()
ENDFUNCTION()
