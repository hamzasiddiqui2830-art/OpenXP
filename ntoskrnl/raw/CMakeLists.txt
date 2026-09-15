# raw Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME raw)
set(TARGET_NAME ntosraw)

set(MODULE_SOURCES
    cleanup.c
    close.c
    create.c
    fileinfo.c
    fsctrl.c
    strucsup.c
    rawdisp.c
    rawinit.c
    readwrit.c
    volinfo.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/raw
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

message(STATUS "Configured raw module: ${TARGET_NAME} with 10 sources")
