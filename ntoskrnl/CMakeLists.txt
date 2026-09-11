# NTOSKRNL CMake build coordinator
# Module-specific source manifests and target declarations live in each
# ntoskrnl/<module>/CMakeLists.txt. No source discovery is performed here.

cmake_minimum_required(VERSION 3.15)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(ntoskrnl C ASM)
endif()

set(NTOSKRNL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

if(DEFINED WRK_ARCH)
    set(_NTOS_ARCH "${WRK_ARCH}")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_NTOS_ARCH amd64)
else()
    set(_NTOS_ARCH i386)
endif()

if(_NTOS_ARCH STREQUAL "x86")
    set(_NTOS_ARCH i386)
endif()

if(_NTOS_ARCH MATCHES "^(amd64|AMD64|x86_64)$")
    set(ARCH amd64)
    set(LMACHINE amd64)
elseif(_NTOS_ARCH MATCHES "^(x86|X86|i386|i686)$")
    set(ARCH i386)
    set(LMACHINE x86)
elseif(_NTOS_ARCH MATCHES "^(ia64|IA64)$")
    set(ARCH ia64)
    set(LMACHINE ia64)
else()
    message(FATAL_ERROR "Unsupported NTOSKRNL architecture: ${_NTOS_ARCH}")
endif()

message(STATUS "NTOSKRNL architecture: ${ARCH}")

set(TOPobj "${CMAKE_BINARY_DIR}/obj/${ARCH}")
set(EXE_PATH "${CMAKE_BINARY_DIR}/EXE")
file(MAKE_DIRECTORY "${TOPobj}")
file(MAKE_DIRECTORY "${EXE_PATH}")

add_compile_definitions(_NTOS_ _NTIFS_ _NTDDK_)
if(ARCH STREQUAL "amd64")
    add_compile_definitions(_AMD64_ AMD64 _WIN64)
elseif(ARCH STREQUAL "i386")
    add_compile_definitions(_X86_ i386)
elseif(ARCH STREQUAL "ia64")
    add_compile_definitions(_IA64_ IA64)
endif()

if(NT_UP)
    add_compile_definitions(NT_UP=${NT_UP})
endif()

if(BUILD_CHECKED_KERNEL)
    add_compile_definitions(NTDEBUG=ntsd)
    set(FREEBUILD 0)
else()
    set(FREEBUILD 1)
endif()

if(NOT NTOS_INCLUDE_DIRS)
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
endif()
include_directories(${NTOS_INCLUDE_DIRS})

# -----------------------------------------------------------------------------
# Module dispatch
# -----------------------------------------------------------------------------
# This list is intentionally explicit. Every module must provide its own
# CMakeLists.txt with an explicit source manifest and target declaration.
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
    arb
    kd64
    ntsym
    nls
    po
    vdm
    verifier
)

foreach(MODULE IN LISTS NTOS_MODULES)
    if(NOT EXISTS "${NTOSKRNL_SOURCE_DIR}/${MODULE}/CMakeLists.txt")
        message(FATAL_ERROR
            "NTOSKRNL module '${MODULE}' is missing its required CMakeLists.txt"
        )
    endif()

    add_subdirectory(
        "${NTOSKRNL_SOURCE_DIR}/${MODULE}"
        "${TOPobj}/${MODULE}"
    )
endforeach()

message(STATUS "NTOSKRNL build configured for ${ARCH}")
