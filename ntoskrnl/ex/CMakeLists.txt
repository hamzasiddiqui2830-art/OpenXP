# Executive module CMakeLists.txt
# The manifest is explicit. ntos-old is used only for the legacy
# implementations selected by the active executive build.

set(MODULE_NAME ex)
set(TARGET_NAME ntos_ex)

set(EX_LOCAL_SOURCE_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}/ex-local)
file(MAKE_DIRECTORY ${EX_LOCAL_SOURCE_DIR})

set(EX_LEGACY_SOURCES mutant.c pool.c pushlock.c sysinfo.c systime.c)
foreach(EX_SOURCE IN LISTS EX_LEGACY_SOURCES)
    configure_file(
        ${REACTOS_SOURCE_DIR}/ntos-old/ex/${EX_SOURCE}
        ${EX_LOCAL_SOURCE_DIR}/${EX_SOURCE}
        COPYONLY
    )
endforeach()

if(MSVC)
    set_source_files_properties(
        ${EX_LOCAL_SOURCE_DIR}/sysinfo.c
        PROPERTIES COMPILE_OPTIONS "/wd4018"
    )
endif()

set(MODULE_SOURCES
    callback.c
    callperf.c
    dbgctrl.c
    delay.c
    event.c
    eventpr.c
    exatom.c
    exdata.c
    exinfo.c
    exinit.c
    exhotp.c
    fmutexc.c
    handle.c
    harderr.c
    hdlsterm.c
    lookasid.c
    luid.c
    ${EX_LOCAL_SOURCE_DIR}/mutant.c
    ${EX_LOCAL_SOURCE_DIR}/pool.c
    ${EX_LOCAL_SOURCE_DIR}/pushlock.c
    nbqueue.c
    probe.c
    profile.c
    raise.c
    resource.c
    ${EX_LOCAL_SOURCE_DIR}/sysinfo.c
    semphore.c
    sysenv.c
    ${EX_LOCAL_SOURCE_DIR}/systime.c
    timer.c
    worker.c
    zone.c
    uuid.c
    win32.c
    rundown.c
    xipdisp.c
    keyedevent.c
)

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

message(STATUS "Configured ex module: ${TARGET_NAME}")
