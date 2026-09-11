# ps Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ps)
set(TARGET_NAME ntosps)

set(MODULE_SOURCES
    psspnd.c
    pscid.c
    psimpers.c
    psinit.c
    psdelete.c
    psquery.c
    create.c
    kulookup.c
    psquota.c
    psctx.c
    psopen.c
    psjob.c
    security.c
    pshelper.c
    psenum.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ps
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ke
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

message(STATUS "Configured ps module: ${TARGET_NAME} with 15 sources")
