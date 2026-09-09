# ex Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ex)
set(TARGET_NAME ntosex)

# Source files from sources.inc
set(MODULE_SOURCES
    callback.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/callperf.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/dbgctrl.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/delay.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/event.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/eventpr.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/exatom.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/exdata.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/exinfo.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/exinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/exhotp.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/fmutexc.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/handle.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/harderr.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/hdlsterm.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/lookasid.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/luid.c

    # These implementations match the executive headers currently selected
    # by this build. The newer converted sources use newer private APIs and
    # data structures that are not present in the active ex.h.
    ${REACTOS_SOURCE_DIR}/ntos-old/ex/mutant.c
    ${REACTOS_SOURCE_DIR}/ntos-old/ex/pool.c
    ${REACTOS_SOURCE_DIR}/ntos-old/ex/pushlock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/nbqueue.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/probe.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/profile.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/raise.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/resource.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/semphore.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/sysenv.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/sysinfo.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/systime.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/timer.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/worker.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/zone.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/uuid.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/win32.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/rundown.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/xipdisp.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex/keyedevent.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex
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

message(STATUS "Configured ex module: ${TARGET_NAME}")
