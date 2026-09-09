# NTOSKRNL CMake Build System
# Converted from legacy Microsoft build system
#
# You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement.
# If you do not agree to the terms, do not use the code.

cmake_minimum_required(VERSION 3.15)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(ntoskrnl C ASM)
endif()

# This CMake project lives below the repository root. Keep one canonical root
# so every module uses stable paths regardless of the build directory.
get_filename_component(REACTOS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(NTOSKRNL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Build configuration options
option(BUILD_CHECKED_KERNEL "Build checked kernel with debug features" OFF)
option(BUILD_NTOSKRNL_TESTS "Build ntoskrnl test targets" OFF)
option(NT_UP "Build uniprocessor kernel (0 for MP)" 1)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Prefer the repository's target architecture selection when present. Fall back
# to the host compiler only for standalone use of this directory.
if(DEFINED WRK_ARCH)
    set(_NTOS_ARCH "${WRK_ARCH}")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_NTOS_ARCH amd64)
else()
    set(_NTOS_ARCH x86)
endif()

if(_NTOS_ARCH MATCHES "^(amd64|AMD64|x86_64)$")
    set(ARCH amd64)
    set(LMACHINE amd64)
    add_compile_definitions(_AMD64_ AMD64 _WIN64)
elseif(_NTOS_ARCH MATCHES "^(x86|X86|i386|i686)$")
    set(ARCH i386)
    set(LMACHINE x86)
    add_compile_definitions(_X86_ i386)
elseif(_NTOS_ARCH MATCHES "^(ia64|IA64)$")
    set(ARCH ia64)
    set(LMACHINE ia64)
    add_compile_definitions(_IA64_ IA64)
else()
    message(FATAL_ERROR "Unsupported NTOSKRNL architecture: ${_NTOS_ARCH}")
endif()

message(STATUS "NTOSKRNL source root: ${REACTOS_SOURCE_DIR}")
message(STATUS "NTOSKRNL architecture: ${ARCH}")

set(TOPobj "${CMAKE_BINARY_DIR}/obj/${ARCH}")
set(EXE_PATH "${CMAKE_BINARY_DIR}/EXE")
file(MAKE_DIRECTORY "${TOPobj}")
file(MAKE_DIRECTORY "${EXE_PATH}")

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

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# Kernel headers must remain independent of the host Windows SDK.  Keep the
# WRK/public headers and the repository's internal SDK ahead of compiler-host
# include directories; ntoskrnl/inc/specstrings.h provides the local annotation
# vocabulary and never imports the host Windows SDK sal.h.
set(NTOS_INCLUDE_DIRS
    ${REACTOS_SOURCE_DIR}
    ${REACTOS_SOURCE_DIR}/base/inc
    ${REACTOS_SOURCE_DIR}/sdk/halkit/inc
    ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
    ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/base/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/sdktools/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
    ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
    ${NTOSKRNL_SOURCE_DIR}/inc
    ${NTOSKRNL_SOURCE_DIR}/rtl
    ${REACTOS_SOURCE_DIR}/ntos-old/rtl
)
include_directories(${NTOS_INCLUDE_DIRS})

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

foreach(MODULE ${MODULES})
    set(MODULE_SOURCE_DIR "${NTOSKRNL_SOURCE_DIR}/${MODULE}")
    if(EXISTS "${MODULE_SOURCE_DIR}/CMakeLists.txt")
        add_subdirectory("${MODULE_SOURCE_DIR}" "${TOPobj}/${MODULE}")
    else()
        message(STATUS "Skipping missing ntoskrnl module: ${MODULE}")
    endif()
endforeach()

message(STATUS "NTOSKRNL build configured for ${ARCH}")
message(STATUS "NTOSKRNL tests: ${BUILD_NTOSKRNL_TESTS}")
message(STATUS "Modules: ${MODULES}")
