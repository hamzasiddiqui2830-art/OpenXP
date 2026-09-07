# ex Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ex)
set(TARGET_NAME ntosex)

# Source files from sources.inc
set(MODULE_SOURCES
    callback.c  
    ..callperf.c
    ..dbgctrl.c
    ..delay.c
    ..event.c
    ..eventpr.c
    ..exatom.c
    ..exdata.c
    ..exinfo.c
    ..exinit.c
    ..exhotp.c
    ..fmutexc.c
    ..handle.c
    ..harderr.c
    ..hdlsterm.c
    ..lookasid.c
    ..luid.c
    ..mutant.c
    ..nbqueue.c
    ..pool.c
    ..probe.c
    ..profile.c
    ..raise.c
    ..resource.c
    ..semphore.c
    ..sysenv.c
    ..sysinfo.c
    ..systime.c
    ..timer.c
    ..worker.c
    ..zone.c
    ..uuid.c
    ..win32.c
    ..rundown.c
    ..pushlock.c
    ..xipdisp.c
    ..keyedevent.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ke
    ${CMAKE_CURRENT_SOURCE_DIR}/../../kd64
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
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
