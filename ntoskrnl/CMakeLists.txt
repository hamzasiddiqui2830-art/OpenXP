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

# The WRK sources.inc files define _NTOS_, _NTIFS_ and _NTDDK_ as part of
# the public-header environment. ntos.h itself owns the corresponding guards,
# so do not inject those guard symbols globally with CMake.
if(ARCH STREQUAL "amd64")
    set(NTOS_ARCH_DEFINES _AMD64_ AMD64 _WIN64)
elseif(ARCH STREQUAL "i386")
    set(NTOS_ARCH_DEFINES _X86_ i386)
elseif(ARCH STREQUAL "ia64")
    set(NTOS_ARCH_DEFINES _IA64_ IA64)
endif()

set(NTOS_COMMON_DEFINES
    ${NTOS_ARCH_DEFINES}
    _NTSYSTEM_
    NTOS_KERNEL_RUNTIME=1
)

if(BUILD_CHECKED_KERNEL)
    list(APPEND NTOS_COMMON_DEFINES NTDEBUG=ntsd)
else()
    list(APPEND NTOS_COMMON_DEFINES NDEBUG)
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

# Make the common WRK modules visible to the kernel-image variant targets.
# Each module owns its explicit source manifest. The image variants below
# select their own compile definitions rather than changing global state.
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
endforeach()

# Common module targets are configured once. Kernel-image targets link the
# corresponding module libraries and carry the WRK image-specific symbols.
foreach(MODULE IN LISTS NTOS_MODULES)
    add_subdirectory(
        "${NTOSKRNL_SOURCE_DIR}/${MODULE}"
        "${TOPobj}/${MODULE}"
    )
endforeach()

if(ARCH STREQUAL "i386")
    set(NTOS_IMAGE_VARIANTS
        ntoskrnl
        ntkrnlmp
        ntkrnlpa
        ntkrpamp
    )

    # The WRK v1.2 x86 source hierarchy explicitly builds UP, MP, PAE and
    # PAE+MP images. PAE adds _X86PAE_ and LARGEADDRESSAWARE; MP clears NT_UP.
    set(NTOS_IMAGE_COMMON_DEFINES
        ${NTOS_COMMON_DEFINES}
        KERNEL_NAME
    )

    add_executable(ntoskrnl ${NTOSKRNL_SOURCE_DIR}/init/ntoskrnl.c)
    add_executable(ntkrnlmp ${NTOSKRNL_SOURCE_DIR}/init/ntkrnlmp.c)
    add_executable(ntkrnlpa ${NTOSKRNL_SOURCE_DIR}/init/ntkrnlpa.c)
    add_executable(ntkrpamp ${NTOSKRNL_SOURCE_DIR}/init/ntkrpamp.c)

    foreach(TARGET_NAME IN LISTS NTOS_IMAGE_VARIANTS)
        target_include_directories(${TARGET_NAME} PRIVATE ${NTOS_INCLUDE_DIRS})
        target_compile_definitions(${TARGET_NAME} PRIVATE ${NTOS_IMAGE_COMMON_DEFINES})
        target_compile_options(${TARGET_NAME} PRIVATE /W3 /WX)
        set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${EXE_PATH}"
            OUTPUT_NAME "${TARGET_NAME}"
        )
    endforeach()

    target_compile_definitions(ntoskrnl PRIVATE NT_UP=1 KERNEL_NAME="ntoskrnl.exe")
    target_compile_definitions(ntkrnlmp PRIVATE NT_UP=0 KERNEL_NAME="ntkrnlmp.exe")
    target_compile_definitions(ntkrnlpa PRIVATE NT_UP=1 _X86PAE_ KERNEL_NAME="ntkrnlpa.exe")
    target_compile_definitions(ntkrpamp PRIVATE NT_UP=0 _X86PAE_ KERNEL_NAME="ntkrpamp.exe")

    # /LARGEADDRESSAWARE is the WRK PAE image link property.
    target_link_options(ntkrnlpa PRIVATE /LARGEADDRESSAWARE)
    target_link_options(ntkrpamp PRIVATE /LARGEADDRESSAWARE)
elseif(ARCH STREQUAL "amd64")
    add_executable(ntoskrnl ${NTOSKRNL_SOURCE_DIR}/init/ntoskrnl.c)
    target_include_directories(ntoskrnl PRIVATE ${NTOS_INCLUDE_DIRS})
    target_compile_definitions(ntoskrnl PRIVATE ${NTOS_COMMON_DEFINES} NT_UP=1 KERNEL_NAME="ntoskrnl.exe")
    target_compile_options(ntoskrnl PRIVATE /W3 /WX)
    set_target_properties(ntoskrnl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${EXE_PATH}"
        OUTPUT_NAME "ntoskrnl"
    )
endif()

message(STATUS "NTOSKRNL build configured for ${ARCH}")
