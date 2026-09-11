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
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/callback.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/callperf.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/dbgctrl.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/delay.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/event.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/eventpr.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/exatom.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/exdata.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/exinfo.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/exinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/exhotp.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/fmutexc.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/handle.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/harderr.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/hdlsterm.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/lookasid.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/luid.c
    ${EX_LOCAL_SOURCE_DIR}/mutant.c
    ${EX_LOCAL_SOURCE_DIR}/pool.c
    ${EX_LOCAL_SOURCE_DIR}/pushlock.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/nbqueue.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/probe.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/profile.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/raise.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/resource.c
    ${EX_LOCAL_SOURCE_DIR}/sysinfo.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/semphore.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/sysenv.c
    ${EX_LOCAL_SOURCE_DIR}/systime.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/timer.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/worker.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/zone.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/uuid.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/win32.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/rundown.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/xipdisp.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/keyedevent.c
)

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

message(STATUS "Configured ex module: ${TARGET_NAME}")
