# RTL module CMakeLists.txt
# The source list is explicit by design; do not glob this directory.

set(MODULE_NAME rtl)
set(TARGET_NAME ntosrtl)

set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/acledit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/add2stra.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/add2strw.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/assert.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/atom.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/avltable.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/bitmap.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/bootstatus.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/checksum.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/cnvint.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/compress.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/debug.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/eballoc.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/environ.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/error.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/excptdbg.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/gentable.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/gen8dot3.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/guid.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/heap.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/imagedir.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtlimagentheader.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/intbits.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/ldrrsrc.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/ldrreloc.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/lookup.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/lznt1.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/message.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/nls.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/nlsxlat.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/pctohdr.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/peb.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/prefix.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/prodtype.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/random.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/range.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/regutil.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtlassig.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtldata.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtlexec.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtlpatch.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rxact.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/sertl.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/splay.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/str2adda.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/str2addw.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/string.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/stktrace.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/sysvol.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/time.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/tracedb.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/version.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/xencode.c
)

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

if(BUILD_RTL_BOOT_TARGETS)
    set(RTL_BOOT_SOURCES
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/assert.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/bitmap.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/cnvint.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/debug.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/imagedir.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/rtlimagentheader.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/checksum.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/compress.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/ldrreloc.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/lznt1.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/time.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/nlsxlat.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/xdecode.c
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl/sertl.c
    )

    add_wrk_module(bldrrtl
        TARGET_NAME bldrrtl
        SOURCES ${RTL_BOOT_SOURCES}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/boot
    )

    add_wrk_module(bldrrtl_dbg
        TARGET_NAME bldrrtl_dbg
        SOURCES ${RTL_BOOT_SOURCES}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/boot_dbg
    )

    target_compile_definitions(bldrrtl PRIVATE BLDR_KERNEL_RUNTIME=1)
    target_compile_definitions(bldrrtl_dbg PRIVATE BLDR_KERNEL_RUNTIME=1)
    set_target_properties(bldrrtl bldrrtl_dbg PROPERTIES
        POSITION_INDEPENDENT_CODE OFF
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}
        OUTPUT_NAME bldrrtl
    )
endif()

message(STATUS "Configured rtl module: ${TARGET_NAME}")
