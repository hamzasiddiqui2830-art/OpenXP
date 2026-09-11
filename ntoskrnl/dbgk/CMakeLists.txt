# dbgk Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME dbgk)
set(TARGET_NAME ntosdbgk)

set(MODULE_SOURCES
    dbgkport.c
    dbgkproc.c
    dbgkobj.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/dbgk
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

message(STATUS "Configured dbgk module: ${TARGET_NAME} with 3 sources")
