# Executive module CMakeLists.txt

set(MODULE_NAME ex)
set(TARGET_NAME ntos_ex)

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
    mutant.c
    pool.c
    pushlock.c
    nbqueue.c
    probe.c
    profile.c
    raise.c
    resource.c
    sysinfo.c
    semphore.c
    sysenv.c
    systime.c
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
