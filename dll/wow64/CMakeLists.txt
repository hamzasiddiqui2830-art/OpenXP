# OpenXP WOW64 Subsystem CMake Configuration

cmake_minimum_required(VERSION 3.15)

# Detect host architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(HOST_ARCH "amd64")
    set(TARGET_ARCH "x86")
    set(WOW64_TARGET "i386")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM|aarch64")
        set(HOST_ARCH "arm64")
        set(TARGET_ARCH "arm")
        set(WOW64_TARGET "arm")
    else()
        message(FATAL_ERROR "WOW64 not supported on 32-bit x86")
    endif()
else()
    message(FATAL_ERROR "Unknown host architecture")
endif()

project(wow64 C CXX)

# WOW64 specific definitions
add_definitions(-D_WOW64_)
add_definitions(-D_WIN32)
add_definitions(-D_WINDLL)
add_definitions(-DNTOS_KERNEL)

# Include directories
include_directories(${REACTOS_SOURCE_DIR}/dll/wow64/inc)
include_directories(${REACTOS_SOURCE_DIR}/sdk/sdk/inc)
include_directories(${REACTOS_SOURCE_DIR}/sdk/sdk/include/reactos)
include_directories(${REACTOS_SOURCE_DIR}/ntoskrnl/inc)

# Source files organized by subsystem (Windows 7 layout)
set(WOW64_SOURCES
    # Core initialization and API mapping
    apimap.c
    
    # CPU emulation layer
    cpu/context.c
    cpu/cpumain.c
    cpu/common.c
    cpu/suspend.c
)

# Create WOW64 DLL
add_library(wow64 SHARED ${WOW64_SOURCES})

# Set output name and directory
set_target_properties(wow64 PROPERTIES
    OUTPUT_NAME "wow64"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output-${WOW64_TARGET}/system32"
)

# Link libraries
target_link_libraries(wow64
    ntdll
    kernel32
)

# Compiler specific flags
if(MSVC)
    target_compile_options(wow64 PRIVATE
        /GS-
        /Gy
        /Ol
        /Oi
        /W3
        /WX-
    )
    
    # Export definitions
    if(EXISTS "${REACTOS_SOURCE_DIR}/dll/wow64/wow64.def")
        set_target_properties(wow64 PROPERTIES
            LINK_FLAGS "/DEF:${REACTOS_SOURCE_DIR}/dll/wow64/wow64.def"
        )
    endif()
else()
    target_compile_options(wow64 PRIVATE
        -fno-stack-protector
        -fomit-frame-pointer
        -O2
        -Wall
        -Wno-unused
    )
endif()

# Architecture specific handling
if(HOST_ARCH STREQUAL "amd64")
    target_compile_definitions(wow64 PRIVATE
        _AMD64_
        _WIN64
        TARGET_X86
    )
elseif(HOST_ARCH STREQUAL "arm64")
    target_compile_definitions(wow64 PRIVATE
        _ARM64_
        _WIN64
        TARGET_ARM
    )
endif()

message(STATUS "WOW64 Configuration:")
message(STATUS "  Host Architecture: ${HOST_ARCH}")
message(STATUS "  Target Architecture: ${TARGET_ARCH}")
message(STATUS "  WOW64 Target: ${WOW64_TARGET}")
message(STATUS "  Output Directory: ${CMAKE_BINARY_DIR}/output-${WOW64_TARGET}/system32")
