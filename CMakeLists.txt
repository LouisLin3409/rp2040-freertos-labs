cmake_minimum_required(VERSION 3.12)

# Name project
SET(ProjectName rp2040-freertos-template)

# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

# Import those libraries
include(pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

# Define project
project(${ProjectName})

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Select Project Folder
if(NOT DEFINED ACTIVE_PROJECT)
    message(FATAL_ERROR "Please define ACTIVE_PROJECT using -DACTIVE_PROJECT=<project_name>")
endif()
set(ACTIVE_PROJECT "project" CACHE STRING "Which project to build in src/")

# Add subdirectories
add_subdirectory(src/${ACTIVE_PROJECT})

# if you have anything in "lib" folder then uncomment below - remember to add a CMakeLists.txt
# file to the "lib" directory
#add_subdirectory(lib)