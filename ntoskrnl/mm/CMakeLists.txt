# mm Module CMakeLists.txt

set(MODULE_NAME mm)
set(TARGET_NAME ntosmm)

set(MODULE_SOURCES
    acceschk.c
    addrsup.c
    allocpag.c
    allocvm.c
    buildmdl.c
    checkpfn.c
    checkpte.c
    compress.c
    crashdmp.c
    creasect.c
    debugsup.c
    deleteva.c
    dmpaddr.c
    dynmem.c
    extsect.c
    flushbuf.c
    flushsec.c
    forksup.c
    freevm.c
    hypermap.c
    iosup.c
    lockvm.c
    mapcache.c
    mapview.c
    miglobal.c
    mirror.c
    mmfault.c
    mminit.c
    mmsup.c
    mmpatch.c
    mmquota.c
    modwrite.c
    nolowmem.c
    pagfault.c
    pfndec.c
    pfnlist.c
    pfsup.c
    physical.c
    procsup.c
    protect.c
    querysec.c
    queryvm.c
    readwrt.c
    sectsup.c
    session.c
    sessload.c
    shutdown.c
    specpool.c
    sysload.c
    sysptes.c
    triage.c
    umapview.c
    vadtree.c
    verifier.c
    wslist.c
    wsmanage.c
    wstree.c
    wrtfault.c
    wrtwatch.c
    zeropage.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/mm
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl/kd64
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    _NTOS_
    _NTIFS_
    _NTDDK_
)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror)
endif()

message(STATUS "Configured mm module: ${TARGET_NAME} with 60 sources")
