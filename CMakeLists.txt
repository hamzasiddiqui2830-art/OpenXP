#
# CMake build system for OpenXP / WRK
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

if(NOT DEFINED WRK_ARCH)
    if(DEFINED ENV{WRK_ARCH})
        set(WRK_ARCH $ENV{WRK_ARCH})
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WRK_ARCH amd64)
    else()
        set(WRK_ARCH x86)
    endif()
endif()

option(BUILD_NTOSKRNL_TESTS "Build ntoskrnl test targets" OFF)
option(BUILD_RTL_BOOT_TARGETS "Build the legacy RTL boot libraries" OFF)
option(BUILD_CHKREG "Build the legacy chkreg user-mode utility" OFF)
message(STATUS "NTOSKRNL tests: ${BUILD_NTOSKRNL_TESTS}")
message(STATUS "RTL boot targets: ${BUILD_RTL_BOOT_TARGETS}")
message(STATUS "chkreg utility: ${BUILD_CHKREG}")

project(wrk VERSION 5.2 LANGUAGES C ASM)

set(REACTOS_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(ENABLE_HAL_ARM "Build ARM HAL" ON)
option(ENABLE_HAL_UP "Build Uniprocessor HAL" OFF)
option(ENABLE_HAL_ACPI "Build ACPI HAL" OFF)
option(ENABLE_HAL_MPS "Build Multiprocessor HAL" OFF)
option(ENABLE_HAL_APIC "Build ACPI APIC HAL" OFF)

list(APPEND CMAKE_MODULE_PATH ${REACTOS_SOURCE_DIR}/cmake/modules)
include(WRKCompilerFlags)
include(WRKModuleBuilder)

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

set(NTOSKRNL_SOURCE_DIR ${REACTOS_SOURCE_DIR}/ntoskrnl)
set(SDK_SOURCE_DIR ${REACTOS_SOURCE_DIR}/sdk)

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

set_wrk_compiler_flags()
set_wrk_include_directories()
add_compile_definitions(${ARCH_DEFINES})

set(RTL_KERNEL_SOURCES
    ${NTOSKRNL_SOURCE_DIR}/rtl/acledit.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/add2stra.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/add2strw.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/assert.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/atom.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/avltable.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/bitmap.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/bootstatus.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/checksum.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/cnvint.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/compress.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/debug.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/eballoc.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/environ.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/error.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/excptdbg.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/gentable.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/gen8dot3.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/guid.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/heap.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/imagedir.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rtlimagentheader.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/intbits.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/ldrrsrc.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/ldrreloc.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/lookup.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/lznt1.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/message.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/nls.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/nlsxlat.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/pctohdr.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/peb.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/prefix.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/prodtype.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/random.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/range.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/regutil.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rtlassig.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rtldata.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rtlexec.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rtlpatch.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/rxact.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/sertl.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/splay.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/str2adda.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/str2addw.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/string.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/stktrace.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/sysvol.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/time.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/tracedb.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/version.c
    ${NTOSKRNL_SOURCE_DIR}/rtl/xencode.c
)

add_wrk_module(rtl TARGET_NAME ntosrtl SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/rtl SOURCES ${RTL_KERNEL_SOURCES})

if(BUILD_RTL_BOOT_TARGETS)
    set(RTL_BOOT_SOURCES
        ${NTOSKRNL_SOURCE_DIR}/rtl/assert.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/bitmap.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/cnvint.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/debug.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/imagedir.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/rtlimagentheader.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/checksum.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/compress.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/ldrreloc.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/lznt1.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/time.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/nlsxlat.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/xdecode.c
        ${NTOSKRNL_SOURCE_DIR}/rtl/sertl.c
    )
    add_wrk_module(bldrrtl TARGET_NAME bldrrtl SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/rtl/boot SOURCES ${RTL_BOOT_SOURCES} OUTPUT_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/boot)
    add_wrk_module(bldrrtl_dbg TARGET_NAME bldrrtl_dbg SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/rtl/boot_dbg SOURCES ${RTL_BOOT_SOURCES} OUTPUT_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/boot_dbg)
    target_compile_definitions(bldrrtl PRIVATE BLDR_KERNEL_RUNTIME=1)
    target_compile_definitions(bldrrtl_dbg PRIVATE BLDR_KERNEL_RUNTIME=1)
    set_target_properties(bldrrtl bldrrtl_dbg PROPERTIES POSITION_INDEPENDENT_CODE OFF ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME} OUTPUT_NAME bldrrtl)
endif()

add_subdirectory(${NTOSKRNL_SOURCE_DIR}/config)

# Stage the older executive implementations into the build tree. They are
# compiled from the generated local directory, never directly from ntos-old.
set(EX_LOCAL_SOURCE_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/ex-local)
file(MAKE_DIRECTORY ${EX_LOCAL_SOURCE_DIR})
set(EX_LEGACY_SOURCES mutant.c pool.c pushlock.c sysinfo.c systime.c)
foreach(EX_SOURCE IN LISTS EX_LEGACY_SOURCES)
    configure_file(${REACTOS_SOURCE_DIR}/ntos-old/ex/${EX_SOURCE} ${EX_LOCAL_SOURCE_DIR}/${EX_SOURCE} COPYONLY)
endforeach()

# The WRK sysinfo implementation has one legacy FIELD_OFFSET comparison
# whose operands have different signedness with the local MSVC headers.
# Keep that compatibility exception scoped to this source file rather than
# weakening /WX for the executive target as a whole.
if(MSVC)
    set_source_files_properties(${EX_LOCAL_SOURCE_DIR}/sysinfo.c PROPERTIES COMPILE_OPTIONS "/wd4018")
endif()

set(EX_SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/ex)
set(EX_KERNEL_SOURCES
    ${EX_SOURCE_DIR}/callback.c
    ${EX_SOURCE_DIR}/callperf.c
    ${EX_SOURCE_DIR}/dbgctrl.c
    ${EX_SOURCE_DIR}/delay.c
    ${EX_SOURCE_DIR}/event.c
    ${EX_SOURCE_DIR}/eventpr.c
    ${EX_SOURCE_DIR}/exatom.c
    ${EX_SOURCE_DIR}/exdata.c
    ${EX_SOURCE_DIR}/exinfo.c
    ${EX_SOURCE_DIR}/exinit.c
    ${EX_SOURCE_DIR}/exhotp.c
    ${EX_SOURCE_DIR}/fmutexc.c
    ${EX_SOURCE_DIR}/handle.c
    ${EX_SOURCE_DIR}/harderr.c
    ${EX_SOURCE_DIR}/hdlsterm.c
    ${EX_SOURCE_DIR}/lookasid.c
    ${EX_SOURCE_DIR}/luid.c
    ${EX_LOCAL_SOURCE_DIR}/mutant.c
    ${EX_LOCAL_SOURCE_DIR}/pool.c
    ${EX_LOCAL_SOURCE_DIR}/pushlock.c
    ${EX_SOURCE_DIR}/nbqueue.c
    ${EX_SOURCE_DIR}/probe.c
    ${EX_SOURCE_DIR}/profile.c
    ${EX_SOURCE_DIR}/raise.c
    ${EX_SOURCE_DIR}/resource.c
    ${EX_LOCAL_SOURCE_DIR}/sysinfo.c
    ${EX_SOURCE_DIR}/semphore.c
    ${EX_SOURCE_DIR}/sysenv.c
    ${EX_LOCAL_SOURCE_DIR}/systime.c
    ${EX_SOURCE_DIR}/timer.c
    ${EX_SOURCE_DIR}/worker.c
    ${EX_SOURCE_DIR}/zone.c
    ${EX_SOURCE_DIR}/uuid.c
    ${EX_SOURCE_DIR}/win32.c
    ${EX_SOURCE_DIR}/rundown.c
    ${EX_SOURCE_DIR}/xipdisp.c
    ${EX_SOURCE_DIR}/keyedevent.c
)

add_wrk_module(ex TARGET_NAME ntos_ex SOURCE_DIR ${EX_SOURCE_DIR} SOURCES ${EX_KERNEL_SOURCES})

# Security is intentionally an explicit source manifest. The directory also
# contains legacy user-mode/test programs that must never enter ntos_se.
set(SE_KERNEL_SOURCES
    ${NTOSKRNL_SOURCE_DIR}/se/accessck.c
    ${NTOSKRNL_SOURCE_DIR}/se/capture.c
    ${NTOSKRNL_SOURCE_DIR}/se/privileg.c
    ${NTOSKRNL_SOURCE_DIR}/se/rmlogon.c
    ${NTOSKRNL_SOURCE_DIR}/se/rmmain.c
    ${NTOSKRNL_SOURCE_DIR}/se/rmvars.c
    ${NTOSKRNL_SOURCE_DIR}/se/seassign.c
    ${NTOSKRNL_SOURCE_DIR}/se/seaudit.c
    ${NTOSKRNL_SOURCE_DIR}/se/sepaudit.c
    ${NTOSKRNL_SOURCE_DIR}/se/seclient.c
    ${NTOSKRNL_SOURCE_DIR}/se/seglobal.c
    ${NTOSKRNL_SOURCE_DIR}/se/seinit.c
    ${NTOSKRNL_SOURCE_DIR}/se/semethod.c
    ${NTOSKRNL_SOURCE_DIR}/se/sep.c
    ${NTOSKRNL_SOURCE_DIR}/se/subject.c
    ${NTOSKRNL_SOURCE_DIR}/se/seastate.c
    ${NTOSKRNL_SOURCE_DIR}/se/token.c
    ${NTOSKRNL_SOURCE_DIR}/se/tokenadj.c
    ${NTOSKRNL_SOURCE_DIR}/se/tokendup.c
    ${NTOSKRNL_SOURCE_DIR}/se/tokenopn.c
    ${NTOSKRNL_SOURCE_DIR}/se/tokenqry.c
    ${NTOSKRNL_SOURCE_DIR}/se/tokenset.c
    ${NTOSKRNL_SOURCE_DIR}/se/adtlog.c
    ${NTOSKRNL_SOURCE_DIR}/se/adtinit.c
    ${NTOSKRNL_SOURCE_DIR}/se/adtvars.c
    ${NTOSKRNL_SOURCE_DIR}/se/adtutil.c
    ${NTOSKRNL_SOURCE_DIR}/se/rmaudit.c
)
add_wrk_module(se TARGET_NAME ntos_se SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/se SOURCES ${SE_KERNEL_SOURCES})

set(NTOS_MODULES ob mm ke ps io cache lpc dbgk raw fstub fsrtl wmi perf init)
foreach(MODULE IN LISTS NTOS_MODULES)
    add_wrk_module(${MODULE} SOURCE_DIR ${NTOSKRNL_SOURCE_DIR}/${MODULE})
endforeach()

if(BUILD_CHKREG)
    add_subdirectory(${NTOSKRNL_SOURCE_DIR}/config/chkreg)
endif()

if(BUILD_NTOSKRNL_TESTS)
    set(NTOSKRNL_TEST_DIRECTORIES ${NTOSKRNL_SOURCE_DIR}/rtl/test ${NTOSKRNL_SOURCE_DIR}/config/test ${NTOSKRNL_SOURCE_DIR}/ke/tests ${NTOSKRNL_SOURCE_DIR}/po/test)
    foreach(TEST_DIRECTORY IN LISTS NTOSKRNL_TEST_DIRECTORIES)
        if(EXISTS "${TEST_DIRECTORY}/CMakeLists.txt")
            add_subdirectory("${TEST_DIRECTORY}")
        endif()
    endforeach()
endif()

set(NTOS_MODULE_LIBS ntosrtl ntosconfig ntos_ex)
foreach(MODULE IN LISTS NTOS_MODULES)
    list(APPEND NTOS_MODULE_LIBS ntos_${MODULE})
endforeach()
list(APPEND NTOS_MODULE_LIBS ntos_se)
if(WRK_ARCH_NAME MATCHES "arm|arm64")
    if(ENABLE_HAL_ARM)
        list(APPEND NTOS_MODULE_LIBS halarm)
    else()
        list(APPEND NTOS_MODULE_LIBS hal_stub)
    endif()
else()
    list(APPEND NTOS_MODULE_LIBS hal_stub)
endif()
if(EXISTS ${REACTOS_SOURCE_DIR}/dll/CMakeLists.txt)
    add_subdirectory(dll)
endif()

# Main kernel executable is intentionally not synthesized until the WRK kernel
# image/startup target has been mapped from its legacy build definition.
