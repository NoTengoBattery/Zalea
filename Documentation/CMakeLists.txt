#===-- CMakeLists.txt - Generate Doxygen Documentation for Project -------------------------------------*- CMake -*-===#
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
#/ Generate Doxygen documentation during build. This target will be present when Doxygen is found.
#/
#===----------------------------------------------------------------------------------------------------------------===#

SET(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}")
SET(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/html/index.html")
SET(DOXYGEN_DOXYFILE_IN "${CMAKE_CURRENT_LIST_DIR}/Doxyfile")
SET(DOXYGEN_DOXYFILE_OUT "${CMAKE_CURRENT_LIST_DIR}/Doxyfile.${CMAKE_BUILD_TYPE}.ignore")
CONFIGURE_FILE(${DOXYGEN_DOXYFILE_IN} ${DOXYGEN_DOXYFILE_OUT} @ONLY)
ADD_CUSTOM_COMMAND(OUTPUT "${DOXYGEN_INDEX_FILE}"
                   DEPENDS "LinkedImage"
                   COMMAND "${CMAKE_DOXYGEN}" "${DOXYGEN_DOXYFILE_OUT}"
                   WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIR}"
                   MAIN_DEPENDENCY "${DOXYGEN_DOXYFILE_OUT}"
                   COMMENT "Generating Doxygen documentation for the project...")
ADD_CUSTOM_TARGET(Doxygen DEPENDS "${DOXYGEN_INDEX_FILE}")
