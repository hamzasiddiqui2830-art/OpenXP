# lpc Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME lpc)
set(TARGET_NAME ntoslpc)

set(MODULE_SOURCES
    lpcinit.c
    lpccreat.c
    lpclistn.c
    lpcconn.c
    lpccompl.c
    lpcsend.c
    lpcrecv.c
    lpcreply.c
    lpcquery.c
    lpcqueue.c
    lpcclose.c
    lpcpriv.c
    lpclog.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpc
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

message(STATUS "Configured lpc module: ${TARGET_NAME} with 13 sources")
