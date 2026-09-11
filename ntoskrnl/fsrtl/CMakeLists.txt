# FsRtl Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME fsrtl)
set(TARGET_NAME ntosFsRtl)

set(MODULE_SOURCES
    DbcsName.c
    FastIo.c
    FaultTol.c
    FileLock.c
    Filter.c
    FiltrCtx.c
    FsFilter.c
    FsRtlPc.c
    LargeMcb.c
    Name.c
    Notify.c
    Oplock.c
    PnP.c
    Unc.c
    Tunnel.c
    StackOvf.c
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
