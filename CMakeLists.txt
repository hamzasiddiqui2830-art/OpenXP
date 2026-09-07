#
# CMake build system for ReactOS 
#
#

cmake_minimum_required(VERSION 3.15)

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Compiler detection - supports MSVC, MinGW, GCC, Clang, LLVM
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

# Architecture configuration via command line or environment
# Usage: cmake -DWRK_ARCH=x86 .. or cmake -DWRK_ARCH=amd64 ..
if(NOT DEFINED WRK_ARCH)
    if(DEFINED ENV{WRK_ARCH})
        set(WRK_ARCH $ENV{WRK_ARCH})
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WRK_ARCH amd64)
    else()
        set(WRK_ARCH x86)
    endif()
endif()

# Project definition
project(wrk VERSION 5.2 LANGUAGES C ASM)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Add CMake modules path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# Include WRK-specific modules
include(WRKCompilerFlags)
include(WRKModuleBuilder)

# Determine architecture based on WRK_ARCH variable
if(WRK_ARCH STREQUAL "amd64")
    set(WRK_ARCH_NAME amd64)
    set(ARCH_DEFINES _WIN64 _AMD64_ AMD64)
elseif(WRK_ARCH STREQUAL "arm")
    set(WRK_ARCH_NAME arm)
    set(ARCH_DEFINES _ARM_ _M_ARM=4 TARGET_ARM)
elseif(WRK_ARCH STREQUAL "arm64")
    set(WRK_ARCH_NAME arm64)
    set(ARCH_DEFINES _ARM64_ _M_ARM64=4 TARGET_ARM64)
else()
    set(WRK_ARCH_NAME x86)
    set(ARCH_DEFINES _X86_=1 i386=1 STD_CALL FPO=0)
endif()

message(STATUS "Building for architecture: ${WRK_ARCH_NAME}")

# Directory structure (mirroring ReactOS)
set(NTOSKRNL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ntoskrnl)
set(SDK_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sdk)

# Output directories - using compiler-architecture naming convention
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

# Create output directories with architecture-specific naming
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/EXE)

# Set WRK compiler flags and include directories
set_wrk_compiler_flags()
set_wrk_include_directories()

# Add architecture-specific defines
add_compile_definitions(${ARCH_DEFINES})

# Module list (from WRK makefile)
set(NTOS_MODULES
    rtl
    config
    ex
    ob
    se
    mm
    ke
    ps
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
)

# Add all NTOS modules as static libraries
foreach(MODULE ${NTOS_MODULES})
    # Replace slashes with underscores for target name
    string(REPLACE "/" "_" MODULE_TARGET "${MODULE}")
    add_wrk_module(${MODULE_TARGET} SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/${MODULE})
endforeach()

# Collect all module libraries for linking
set(NTOS_MODULE_LIBS "")
foreach(MODULE ${NTOS_MODULES})
    string(REPLACE "/" "_" MODULE_TARGET "${MODULE}")
    list(APPEND NTOS_MODULE_LIBS ntos_${MODULE_TARGET})
endforeach()

# Add HAL stub to the module libs (provides HAL implementations without DLL dependency)
list(APPEND NTOS_MODULE_LIBS hal_stub)

# Main kernel executable
# Note: The actual kernel entry point and startup code would need to be properly configured
# This is a simplified version that demonstrates the CMake structure

# Check for kernel startup code
if(EXISTS ${NTOSKRNL_SOURCE_DIR}/BUILD/ki${WRK_ARCH_NAME}.c)
    set(KERNEL_STARTUP ${NTOSKRNL_SOURCE_DIR}/BUILD/ki${WRK_ARCH_NAME}.c)
elseif(EXISTS ${NTOSKRNL_SOURCE_DIR}/ke/BUILD/ki${WRK_ARCH_NAME}.c)
    set(KERNEL_STARTUP ${NTOSKRNL_SOURCE_DIR}/ke/BUILD/ki${WRK_ARCH_NAME}.c)
else()
    # Create a minimal placeholder if no startup code found
    set(KERNEL_STARTUP ${CMAKE_BINARY_DIR}/kernel_startup.c)
    file(WRITE ${KERNEL_STARTUP} "void KiSystemStartup(void *Block) { while(1); }")
endif()

add_executable(wrk${WRK_ARCH_NAME} ${KERNEL_STARTUP})

# Set kernel entry point based on compiler
if(MSVC)
    if(WRK_ARCH_NAME STREQUAL "x86")
        set_target_properties(wrk${WRK_ARCH_NAME} PROPERTIES
            LINK_FLAGS "/ENTRY:KiSystemStartup@4"
        )
    else()
        set_target_properties(wrk${WRK_ARCH_NAME} PROPERTIES
            LINK_FLAGS "/ENTRY:KiSystemStartup"
        )
    endif()
else()
    # GCC/MinGW/Clang - use linker flags
    if(WRK_ARCH_NAME STREQUAL "x86")
        set_target_properties(wrk${WRK_ARCH_NAME} PROPERTIES
            LINK_FLAGS "-Wl,--entry,KiSystemStartup@4"
        )
    else()
        set_target_properties(wrk${WRK_ARCH_NAME} PROPERTIES
            LINK_FLAGS "-Wl,--entry,KiSystemStartup"
        )
    endif()
endif()

# Configure kernel output
set_target_properties(wrk${WRK_ARCH_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/EXE
    OUTPUT_NAME wrk${WRK_ARCH_NAME}
    SUFFIX ".exe"
    LINK_LIBRARIES "${NTOS_MODULE_LIBS}"
)


add_subdirectory(hal)

# Add pre-built resources if they exist
if(EXISTS ${NTOSKRNL_SOURCE_DIR}/BUILD/PREBUILT/${WRK_ARCH_NAME})
    include_directories(${NTOSKRNL_SOURCE_DIR}/BUILD/PREBUILT/${WRK_ARCH_NAME})
endif()

# Installation rules
install(TARGETS wrk${WRK_ARCH_NAME}
    RUNTIME DESTINATION bin
)

# Custom targets for clean and rebuild
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/obj
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/EXE
    COMMENT "Cleaning all build artifacts"
)

add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Rebuilding entire project"
)

# Print configuration summary
message(STATUS "=== WRK CMake Configuration Summary ===")
message(STATUS "Architecture: ${WRK_ARCH_NAME}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "NTOSKRNL Source: ${NTOSKRNL_SOURCE_DIR}")
message(STATUS "SDK Source: ${SDK_SOURCE_DIR}")
message(STATUS "Output Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Modules: ${NTOS_MODULES}")
message(STATUS "===========================================")
