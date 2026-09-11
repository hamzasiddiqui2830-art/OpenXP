# kd Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME kd)
set(TARGET_NAME ntoskd)

set(MODULE_SOURCES
    file.c
    kdinit.c
    kdbreak.c
    kdlock.c
    kdmove.c
    kddbgio.c
    kdapi.c
    kddata.c
    print.c
    query.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/kd64
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl/mm
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

message(STATUS "Configured kd module: ${TARGET_NAME} with 10 sources")
