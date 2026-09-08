# rtl Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME rtl)
set(TARGET_NAME ntosrtl)

# Source files from rtl directory (main kernel mode RTL library)
set(MODULE_SOURCES
    acledit.c
    add2stra.c
    add2strw.c
    assert.c
    atom.c
    avltable.c
    bitmap.c
    bootstatus.c
    byteswap.c
    checksum.c
    cnvint.c
    compress.c
    debug.c
    eballoc.c
    environ.c
    error.c
    excptdbg.c
    gen8dot3.c
    generr.c
    gentable.c
    guid.c
    handle.c
    heap.c
    heapdbg.c
    heapdll.c
    heapleak.c
    heaplowf.c
    heappage.c
    imagedir.c
    intbits.c
    ldrreloc.c
    ldrrsrc.c
    lookasid.c
    lookup.c
    lznt1.c
    message.c
    mrcf.c
    nls.c
    nlsxlat.c
    pctohdr.c
    peb.c
    prefix.c
    prodtype.c
    random.c
    range.c
    registry.c
    regutil.c
    remlock.c
    rtlassig.c
    rtldata.c
    rtldowncaseunicodechar.c
    rtlexec.c
    rtlfindcharinunicodestring.c
    rtlimagentheader.c
    rtlmmapio.c
    rtlnthdr.c
    rtloverflow.c
    rtlpatch.c
    rtlvalidateunicodestring.c
    rxact.c
    secmem.c
    sertl.c
    slistfunc.c
    splay.c
    stktrace.c
    str2adda.c
    str2addw.c
    string.c
    sysvol.c
    threads.c
    time.c
    timer.c
    tracedb.c
    triangle.c
    version.c
    wait.c
    worker.c
    xdecode.c
    xencode.c
)

# Create library target for kernel-mode RTL
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)

# Compile definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    _NTOS_
    _NTIFS_
    _NTDDK_
)

# Compiler options - match legacy build settings (/W4 /WX = all warnings + warnings as errors)
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror)
endif()

# Include test subdirectory for separate test targets
add_subdirectory(test)

message(STATUS "Configured rtl module: ${TARGET_NAME}")
