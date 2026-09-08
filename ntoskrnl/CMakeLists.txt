# NTOSKRNL CMake Build System
# Converted from legacy Microsoft build system
#
# You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement.
# If you do not agree to the terms, do not use the code.

cmake_minimum_required(VERSION 3.15)
project(ntoskrnl C ASM)

# This CMake project lives below the repository root. Keep one canonical root
# so every module uses stable paths regardless of the build directory.
get_filename_component(REACTOS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(NTOSKRNL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Build configuration options
option(BUILD_CHECKED_KERNEL "Build checked kernel with debug features" OFF)
option(BUILD_NTOSKRNL_TESTS "Build ntoskrnl test targets" OFF)
option(NT_UP "Build uniprocessor kernel (0 for MP)" 1)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Architecture detection
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64)$")
    set(ARCH amd64)
    set(LMACHINE amd64)
    add_compile_definitions(_AMD64_ AMD64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|X86|i[3-6]86)$")
    set(ARCH i386)
    set(LMACHINE x86)
    add_compile_definitions(_X86_ i386)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(IA64|ia64)$")
    set(ARCH ia64)
    set(LMACHINE ia64)
    add_compile_definitions(_IA64_ IA64)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "NTOSKRNL source root: ${REACTOS_SOURCE_DIR}")
message(STATUS "NTOSKRNL architecture: ${ARCH}")

# Build paths
set(TOPobj "${CMAKE_BINARY_DIR}/obj${ARCH}")
set(EXE_PATH "${CMAKE_BINARY_DIR}/EXE")

# Create output directories
file(MAKE_DIRECTORY "${TOPobj}")
file(MAKE_DIRECTORY "${EXE_PATH}")

# Common compiler definitions
add_compile_definitions(_NTOS_ _NTIFS_ _NTDDK_)

if(NT_UP)
    add_compile_definitions(NT_UP=${NT_UP})
endif()

if(BUILD_CHECKED_KERNEL)
    add_compile_definitions(NTDEBUG=ntsd)
    set(FREEBUILD 0)
else()
    set(FREEBUILD 1)
endif()

# Warning level: /W4 is MSVC-only. GCC/Clang use -Wall/-Wextra.
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Include directories shared by all ntoskrnl modules.
set(NTOS_INCLUDE_DIRS
    ${REACTOS_SOURCE_DIR}
    ${NTOSKRNL_SOURCE_DIR}
    ${NTOSKRNL_SOURCE_DIR}/inc
    ${NTOSKRNL_SOURCE_DIR}/ke
    ${NTOSKRNL_SOURCE_DIR}/kd64
    ${NTOSKRNL_SOURCE_DIR}/rtl
)

include_directories(${NTOS_INCLUDE_DIRS})

# ============================================================================
# Module definitions - converted from sources.inc files
# ============================================================================

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

# Include all module CMakeLists. The binary directory mirrors the source
# module path, avoiding collisions between nested modules.
foreach(MODULE ${MODULES})
    if(EXISTS "${REACTOS_SOURCE_DIR}/ntoskrnl/${MODULE}/CMakeLists.txt")
        add_subdirectory(
            "${REACTOS_SOURCE_DIR}/ntoskrnl/${MODULE}"
            "${TOPobj}/${MODULE}"
        )
    else()
        message(STATUS "Skipping missing ntoskrnl module: ${MODULE}")
    endif()
endforeach()

message(STATUS "NTOSKRNL build configured for ${ARCH}")
message(STATUS "NTOSKRNL tests: ${BUILD_NTOSKRNL_TESTS}")
message(STATUS "Modules: ${MODULES}")
