cmake_minimum_required(VERSION 3.22)
include(ExternalProject)

set(CMAKE_DIR ${CMAKE_SOURCE_DIR}/cmake)
#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# In order to use CMake for cross-compiling
set(CMAKE_CXX_COMPILER_FORCED "true") 

# Include toolchain file
include("gcc-arm-none-eabi.cmake")

# Set the project name
set(CMAKE_PROJECT_NAME STM32H7R7L8)

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Include external project file
include("mx-generated.cmake")

set(MCU_NAME STM32H7R7L8)
set(PROJECT_APP_BINARY_DIR ${CMAKE_SOURCE_DIR}/Appli/build)
set(PROJECT_BOOT_BINARY_DIR ${CMAKE_SOURCE_DIR}/Boot/build)
include(${CMAKE_DIR}/gcc-jlink.cmake)

# Astyle
# --------------------------------------------------------------------

# Define check_format target
add_custom_target(check_format
    COMMAND ${CMAKE_COMMAND} -E echo "Checking formatting with astyle"
    COMMAND ${ASTYLE_EXECUTABLE} Tools/astyle/check_code_style_all.sh
    COMMAND ${CMAKE_COMMAND} -E echo "Running git diff to check for changes"
    COMMAND git diff --check
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Define format target
add_custom_target(format
    COMMAND ${CMAKE_COMMAND} -E echo "Formatting with astyle"
    COMMAND ${ASTYLE_EXECUTABLE} Tools/astyle/check_code_style_all.sh
            --fix
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)