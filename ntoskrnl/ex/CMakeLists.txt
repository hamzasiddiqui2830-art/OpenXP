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

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mutant.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/nbqueue.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pool.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/probe.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/profile.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/raise.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/resource.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/semphore.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sysenv.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sysinfo.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/systime.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/timer.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/worker.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/zone.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/uuid.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/win32.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/rundown.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pushlock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/xipdisp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/keyedevent.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ex
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl/kd64
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT "exp.obj"
# )

message(STATUS "Configured ex module: ${TARGET_NAME} with 37 sources")
