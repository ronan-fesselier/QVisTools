#==============================================================================
# QVisTools - Top-level CMake configuration
#==============================================================================

#----------------------------------------------
# Project definition
#----------------------------------------------

cmake_minimum_required(VERSION 3.25)

project(QVisTools
    VERSION 0.1.0
    DESCRIPTION "Standalone Qt desktop toolkit for DNV's Vessel Information Structure (VIS)"
    HOMEPAGE_URL "https://github.com/ronan-fesselier/QVisTools"
    LANGUAGES CXX
)

#----------------------------------------------
# C++ standard
#----------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DEBUG_POSTFIX "-d")

#----------------------------------------------
# Directory paths
#----------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#----------------------------------------------
# Build type
#----------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

#----------------------------------------------
# Build options
#----------------------------------------------

option(QVIS_TOOLS_WARNINGS           "Enable strict compiler warnings"   OFF)
option(QVIS_TOOLS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

#----------------------------------------------
# CMake modules configuration
#----------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(QVisToolsTargets)
