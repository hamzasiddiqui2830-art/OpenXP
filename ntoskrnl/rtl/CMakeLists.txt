# rtl Module CMakeLists.txt
# Exact kernel RTL source set from ntos-old/rtl/up/sources

set(MODULE_NAME rtl)
set(TARGET_NAME ntosrtl)

set(MODULE_SOURCES
    acledit.c
    add2stra.c
    add2strw.c
    assert.c
    atom.c
    avltable.c
    bitmap.c
    bootstatus.c
    checksum.c
    cnvint.c
    compress.c
    debug.c
    eballoc.c
    environ.c
    error.c
    excptdbg.c
    gentable.c
    gen8dot3.c
    guid.c
    heap.c
    imagedir.c
    rtlimagentheader.c
    intbits.c
    ldrrsrc.c
    ldrreloc.c
    lookup.c
    lznt1.c
    message.c
    nls.c
    nlsxlat.c
    pctohdr.c
    peb.c
    prefix.c
    prodtype.c
    random.c
    range.c
    regutil.c
    rtlassig.c
    rtldata.c
    rtlexec.c
    rtlpatch.c
    rxact.c
    sertl.c
    splay.c
    str2adda.c
    str2addw.c
    string.c
    stktrace.c
    sysvol.c
    time.c
    tracedb.c
    version.c
    xencode.c
)

# Legacy target:
#   TARGETNAME=ntosrtl
#   TARGETTYPE=LIBRARY
#   C_DEFINES += -DNTOS_KERNEL_RUNTIME=1
#   INCLUDES = ..; ../../inc; $(DDK_INC_PATH); $(DS_INC_PATH); $(SDKTOOLS_INC_PATH)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/sdktools/inc
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    NTOS_KERNEL_RUNTIME=1
)

if(MSVC AND WRK_ARCH_NAME STREQUAL "x86")
    target_compile_options(${TARGET_NAME} PRIVATE /Gz /wd4101)
endif()

if(BUILD_NTOSKRNL_TESTS)
    add_subdirectory(test)
endif()

message(STATUS "Configured RTL target: ${TARGET_NAME} (legacy ntosrtl)")
