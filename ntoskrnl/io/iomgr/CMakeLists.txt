# iomgr Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME iomgr)
set(TARGET_NAME ntosiomgr)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/arcsec.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/cancelapi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/complete.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/create.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/devctrl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/dev2dos.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/dumpctl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/dir.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/errorlog.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/fsctrl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/internal.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/iodata.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ioinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/iosubs.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ioverifier.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ioPerf.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/loadunld.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/lock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/misc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/objsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/open.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/parse.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/qsea.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/qsfs.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/qsinfo.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/qsquota.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/query.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/read.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/triage.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/write.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/iomgr
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT "iomgr.obj"
# )

message(STATUS "Configured iomgr module: ${TARGET_NAME} with 30 sources")
