# FsRtl Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME fsrtl)
set(TARGET_NAME ntosFsRtl)

set(MODULE_SOURCES
    dbcsname.c
    fastio.c
    faulttol.c
    filelock.c
    filter.c
    filtrctx.c
    fsfilter.c
    fsrtlpc.c
    largemcb.c
    name.c
    notify.c
    oplock.c
    pnp.c
    unc.c
    tunnel.c
    stackovf.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/fsrtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
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

message(STATUS "Configured FsRtl module: ${TARGET_NAME} with 16 sources")
