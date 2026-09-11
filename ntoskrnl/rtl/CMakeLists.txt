# RTL module CMakeLists.txt
# The source list is explicit by design; do not glob this directory.

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

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

if(BUILD_RTL_BOOT_TARGETS)
    set(RTL_BOOT_SOURCES
        assert.c
        bitmap.c
        cnvint.c
        debug.c
        imagedir.c
        rtlimagentheader.c
        checksum.c
        compress.c
        ldrreloc.c
        lznt1.c
        time.c
        nlsxlat.c
        xdecode.c
        sertl.c
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
