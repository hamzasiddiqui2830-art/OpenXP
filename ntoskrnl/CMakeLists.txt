# NTOSKRNL CMake Build System
# Converted from legacy Microsoft build system
# 
# You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement.
# If you do not agree to the terms, do not use the code.

cmake_minimum_required(VERSION 3.15)
project(ntoskrnl C ASM)

# Build configuration options
option(BUILD_CHECKED_KERNEL "Build checked kernel with debug features" OFF)
option(BUILD_NTOSKRNL_TESTS "Build ntoskrnl test targets" OFF)
option(NT_UP "Build uniprocessor kernel (0 for MP)" 1)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Architecture detection
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(ARCH amd64)
    set(LMACHINE amd64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i386" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "i686")
    set(ARCH i386)
    set(LMACHINE x86)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "IA64")
    set(ARCH ia64)
    set(LMACHINE ia64)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "Building for architecture: ${ARCH}")

# Build paths
set(TOPobj "${CMAKE_BINARY_DIR}/obj${ARCH}")
set(EXE_PATH "${CMAKE_BINARY_DIR}/EXE")

# Create output directories
file(MAKE_DIRECTORY "${TOPobj}")
file(MAKE_DIRECTORY "${EXE_PATH}")

# Common compiler definitions
add_definitions(-D_NTOS_)
add_definitions(-D_NTIFS_)
add_definitions(-D_NTDDK_)

if(NT_UP)
    add_definitions(-DNT_UP=${NT_UP})
endif()

if(BUILD_CHECKED_KERNEL)
    add_definitions(-DNTDEBUG=ntsd)
    set(FREEBUILD 0)
else()
    set(FREEBUILD 1)
endif()

# Warning level
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")

# Include directories
set(NTOS_INCLUDE_DIRS
    ${REACTOS_SOURCE_DIR}
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ke
    ${REACTOS_SOURCE_DIR}/kd64
    ${REACTOS_SOURCE_DIR}/rtl
)

# ============================================================================
# Module definitions - converted from sources.inc files
# ============================================================================

# List of all modules to build
set(MODULES
    rtl
    config
    ex
    ob
    se
    mm
    ke
    ps
    io/iomgr
    io/pnpmgr
    io
    cache
    lpc
    dbgk
    raw
    fstub
    fsrtl
    wmi
    perf
    init
    arb
    kd64
    ntsym
    nls
    po
    vdm
    verifier
)

# Include all module CMakeLists
foreach(MODULE ${MODULES})
    if(EXISTS "${REACTOS_SOURCE_DIR}/${MODULE}/CMakeLists.txt")
        add_subdirectory(${MODULE} ${TOPobj}/${MODULE})
    endif()
endforeach()

# ============================================================================
# Final kernel linking
# ============================================================================

# The final kernel executable will be created by collecting all module libraries
# This is handled in the BUILD subdirectory

message(STATUS "NTOSKRNL build configured for ${ARCH}")
message(STATUS "NTOSKRNL tests: ${BUILD_NTOSKRNL_TESTS}")
message(STATUS "Modules: ${MODULES}")
