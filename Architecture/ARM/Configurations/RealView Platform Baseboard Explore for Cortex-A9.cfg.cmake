#===-- Default Config - A CMake file for a machine default config  ------------------------------------*- CMake -*-===//
#
# This file may be hand crafted or generated by the build system as-is; and is tracked by the version system.
# @formatter:off
#
#===---------------------------------------------------------------------------------------------------------------===//
#/
#/ \file
#/ Contains the current configuration, exported using the SET_AND_EXPORT and SET_AND_EXPORT_FORCE directives. This file
#/ has a correct CMake syntax, therefore, it can be copied and used directly as an initial machine configuration.
#/
#/ Default configurations for machines in the Architecture folder may be hand crafted from a generated file by the build
#/ system, and they are tracked by the build system. They are not "automatically updated", and they are part of the
#/ build system. As such, they must be high quality as the whole project.
#/
#/ This is the default header and all files must contain it, replacing the header of the automatically generated and
#/ updated files by the build system (known as "seeds").
#/
#===---------------------------------------------------------------------------------------------------------------===//

# MACHINE_MARCH: SET_AND_EXPORT STRING
SET(MACHINE_MARCH "armv7-a"
    CACHE STRING
    "This variable is the machine minimum iteration of the ISA for the compiler and binutils.")
# MACHINE_MTUNE: SET_AND_EXPORT STRING
SET(MACHINE_MTUNE "cortex-a9"
    CACHE STRING
    "This variable is the default CPU for the compiler and binutils to tune the performance.")
# MACHINE_NAME: SET_AND_EXPORT STRING
SET(MACHINE_NAME "ARM RealView Platform Baseboard Explore for Cortex-A9"
    CACHE STRING
    "This variable is the machine for the build, a human readable name for the machine.")
