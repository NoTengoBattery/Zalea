#===-- PureGenerateTargetList.cmake - Generate All Possible Target Triples -----------------------------*- CMake -*-===#
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
#/ This function will return a list of all possible target triples, then the CMake Toolchain will try to find tools one
#/ by one.
#/
#/ This extension provides:
#/ -> TARGET_LIST
#/
#===----------------------------------------------------------------------------------------------------------------===#

FUNCTION(TARGET_TRIPLE_LIST RESULT)
  #SET(TEMP_LIST ${KERNEL_TARGET} ${KERNEL_ALTERNATIVE_TARGET} ${KERNEL_SECOND_TARGET})
  FOREACH (arch ${ARCHS})
    FOREACH (sub ${SUBS})
      IF (NOT sub STREQUAL -)
        SET(A_ARCH "${arch}${sub}")
      ELSE ()
        SET(A_ARCH "${arch}")
      ENDIF ()
      FOREACH (vendor ${VENDORS})
        IF (NOT vendor STREQUAL -)
          SET(V_VENDOR "${A_ARCH}-${vendor}")
        ELSE ()
          SET(V_VENDOR "${A_ARCH}")
        ENDIF ()
        FOREACH (sys ${SYSS})
          IF (NOT sys STREQUAL -)
            SET(S_SYS "${V_VENDOR}-${sys}")
          ELSE ()
            SET(S_SYS "${V_VENDOR}")
          ENDIF ()
          FOREACH (abi ${ABIS})
            IF (NOT abi STREQUAL -)
              SET(A_ABI "${S_SYS}-${abi}")
            ELSE ()
              SET(A_ABI "${S_SYS}")
            ENDIF ()
            LIST(APPEND TEMP_LIST "${A_ABI}")
          ENDFOREACH ()
        ENDFOREACH ()
      ENDFOREACH ()
    ENDFOREACH ()
  ENDFOREACH ()
  SET("${RESULT}" ${TEMP_LIST} PARENT_SCOPE)
ENDFUNCTION()