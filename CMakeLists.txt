#
# CMake build system for OpenXP
#

cmake_minimum_required(VERSION 3.15)

# CMake 4.4 introduced CMP0194, which changes how MSVC is classified for
# the ASM language. This project intentionally uses MSVC's cl.exe to assemble
# the legacy .asm sources, so retain the pre-4.4 behavior when the policy is
# available. Older CMake versions simply ignore this block.
if(POLICY CMP0194)
    cmake_policy(SET CMP0194 OLD)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

project(wrk VERSION 5.2 LANGUAGES C ASM)

set(REACTOS_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# -----------------------------------------------------------------------------
# Global build options
# -----------------------------------------------------------------------------
option(BUILD_CHECKED_KERNEL "Build checked kernel with debug features" OFF)
option(BUILD_NTOSKRNL_TESTS "Build ntoskrnl test targets" OFF)
option(BUILD_RTL_BOOT_TARGETS "Build the legacy RTL boot libraries" OFF)
option(BUILD_CHKREG "Build the legacy chkreg user-mode utility" OFF)
option(NT_UP "Build uniprocessor kernel (0 for MP)" ON)

option(ENABLE_HAL_ARM "Build ARM HAL" ON)
option(ENABLE_HAL_UP "Build Uniprocessor HAL" OFF)
option(ENABLE_HAL_ACPI "Build ACPI HAL" OFF)
option(ENABLE_HAL_MPS "Build Multiprocessor HAL" OFF)
option(ENABLE_HAL_APIC "Build ACPI APIC HAL" OFF)

set(WRK_COMPILER "vs" CACHE STRING "WRK compiler (vs or gcc)")

# -----------------------------------------------------------------------------
# Architecture
# -----------------------------------------------------------------------------
if(NOT DEFINED WRK_ARCH OR WRK_ARCH STREQUAL "")
    if(DEFINED ENV{WRK_ARCH} AND NOT "$ENV{WRK_ARCH}" STREQUAL "")
        set(WRK_ARCH "$ENV{WRK_ARCH}")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WRK_ARCH amd64)
    else()
        set(WRK_ARCH i386)
    endif()
endif()

if(WRK_ARCH STREQUAL "x86")
    set(WRK_ARCH i386)
endif()

if(WRK_ARCH STREQUAL "amd64")
    set(WRK_ARCH_NAME amd64)
    set(ARCH_DEFINES _WIN64 _AMD64_ AMD64)
elseif(WRK_ARCH STREQUAL "arm")
    set(WRK_ARCH_NAME arm)
    set(ARCH_DEFINES _ARM_ _M_ARM=4 TARGET_ARM)
elseif(WRK_ARCH STREQUAL "arm64")
    set(WRK_ARCH_NAME arm64)
    set(ARCH_DEFINES _ARM64_ _M_ARM64=4 TARGET_ARM64)
elseif(WRK_ARCH STREQUAL "i386")
    set(WRK_ARCH_NAME i386)
    set(ARCH_DEFINES _X86_=1 i386=1 STD_CALL FPO=0)
else()
    message(FATAL_ERROR "Unsupported WRK architecture: ${WRK_ARCH}")
endif()

message(STATUS "Building for architecture: ${WRK_ARCH_NAME}")

# -----------------------------------------------------------------------------
# Compiler information
# -----------------------------------------------------------------------------
set(DETECTED_COMPILER "")
if(MSVC)
    set(DETECTED_COMPILER "MSVC")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(DETECTED_COMPILER "MinGW")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(DETECTED_COMPILER "GCC")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_LINKER MATCHES "lld")
    set(DETECTED_COMPILER "LLVM")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(DETECTED_COMPILER "Clang")
else()
    set(DETECTED_COMPILER "Unknown")
endif()

message(STATUS "Detected compiler: ${DETECTED_COMPILER}")
message(STATUS "WRK Compiler: ${WRK_COMPILER}")
message(STATUS "WRK Architecture: ${WRK_ARCH_NAME}")
message(STATUS "NTOSKRNL tests: ${BUILD_NTOSKRNL_TESTS}")
message(STATUS "RTL boot targets: ${BUILD_RTL_BOOT_TARGETS}")
message(STATUS "chkreg utility: ${BUILD_CHKREG}")

# -----------------------------------------------------------------------------
# Common build infrastructure
# -----------------------------------------------------------------------------
if(MSVC)
    set(COMPILER_PREFIX "VS")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(COMPILER_PREFIX "MinGW")
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(COMPILER_PREFIX "GCC")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    if(CMAKE_LINKER MATCHES "lld")
        set(COMPILER_PREFIX "LLVM")
    else()
        set(COMPILER_PREFIX "Clang")
    endif()
else()
    set(COMPILER_PREFIX "Unknown")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/EXE)

list(APPEND CMAKE_MODULE_PATH ${REACTOS_SOURCE_DIR}/cmake/modules)
include(WRKCompilerFlags)
include(WRKModuleBuilder)

set(NTOSKRNL_SOURCE_DIR ${REACTOS_SOURCE_DIR}/ntoskrnl)
set(SDK_SOURCE_DIR ${REACTOS_SOURCE_DIR}/sdk)

set_wrk_compiler_flags()
set_wrk_include_directories()
add_compile_definitions(${ARCH_DEFINES})

# -----------------------------------------------------------------------------
# NTOSKRNL
# -----------------------------------------------------------------------------
# Module-specific source manifests and declarations live under
# ntoskrnl/<module>/CMakeLists.txt. The NTOSKRNL CMakeLists only dispatches to
# those manifests; it never discovers sources from the filesystem.
add_subdirectory(${NTOSKRNL_SOURCE_DIR})
