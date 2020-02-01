#===-- Default Configuration - A CMake File which Contains a Default Configuration ---------------------*- CMake -*-===#
#
# This file is a CMake Initial Cache file, which may be hand crafted or generated by the build system. This file should
# be tracked by the version control system, and it's part of the project as the build system will recognize this file as
# a requirement for continuing the configuration process.
#
# @formatter:off
#
#===----------------------------------------------------------------------------------------------------------------===#
#/
#/ \file
#/ Contains the current configuration, exported using the SET_AND_EXPORT and SET_AND_EXPORT_FORCE directives. This file
#/ has a correct CMake syntax, therefore, it can be copied and used directly as an CMake Initial Cache.
#/
#/ Default configurations for machines in the Architecture folder may be hand crafted from a generated file by the build
#/ system, and they are tracked by the build system. They are not "automatically updated", and they are part of the
#/ build system. As such, they must be high quality files. The original format from the generated file is fine.
#/
#/ This is the default header and all default configuration files must contain it, replacing the header of the 
#/ automatically generated and updated files by the build system (known as "seeds").
#/
#===----------------------------------------------------------------------------------------------------------------===#

# MACHINE_LOADABLE_IMAGE_NAME: SET_AND_EXPORT STRING
SET(MACHINE_LOADABLE_IMAGE_NAME "kernel.img"
    CACHE STRING
    "This will set the name of the binary that the machine can run. Some machines require a specific file name.")
# MACHINE_LOAD_ADDRESS: SET_AND_EXPORT STRING
SET(MACHINE_LOAD_ADDRESS "0x8000"
    CACHE STRING
    "This is the load address of the image, used by the loader to place the image in memory.")
# MACHINE_MARCH: SET_AND_EXPORT STRING
SET(MACHINE_MARCH "armv7-a"
    CACHE STRING
    "This variable is the machine minimum iteration of the ISA for the compiler and binutils.")
# MACHINE_MTUNE: SET_AND_EXPORT STRING
SET(MACHINE_MTUNE "cortex-a15"
    CACHE STRING
    "This variable is the default CPU for the compiler and binutils to tune the performance.")
# MACHINE_NAME: SET_AND_EXPORT STRING
SET(MACHINE_NAME "ARM Versatile Express for Cortex-A15"
    CACHE STRING
    "This variable is the machine for the build, a human readable name for the machine.")
# MACHINE_VIRTUAL_ADDRESS: SET_AND_EXPORT STRING
SET(MACHINE_VIRTUAL_ADDRESS "0x8000"
    CACHE STRING
    "This is the 'virtual' address, which is the one that is referenced by the compiled code.")
