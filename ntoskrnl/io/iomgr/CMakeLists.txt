# iomgr Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME iomgr)
set(TARGET_NAME ntosiomgr)

# Source files from sources.inc
set(MODULE_SOURCES
    ..arcsec.c
    ..cancelapi.c
    ..complete.c
    ..create.c
    ..devctrl.c
    ..dev2dos.c
    ..dumpctl.c
    ..dir.c
    ..errorlog.c
    ..fsctrl.c
    ..internal.c
    ..iodata.c
    ..ioinit.c
    ..iosubs.c
    ..ioverifier.c
    ..ioPerf.c
    ..loadunld.c
    ..lock.c
    ..misc.c
    ..objsup.c
    ..open.c
    ..parse.c
    ..qsea.c
    ..qsfs.c
    ..qsinfo.c
    ..qsquota.c
    ..query.c
    ..read.c
    ..triage.c
    ..write.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
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
#     COMPILE_PCH_OUTPUT_INIT "iomgr.obj"
# )

message(STATUS "Configured iomgr module: ${TARGET_NAME} with 30 sources")
