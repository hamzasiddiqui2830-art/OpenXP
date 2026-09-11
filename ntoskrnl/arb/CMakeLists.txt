# arb Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME arb)
set(TARGET_NAME ntosarb)

set(MODULE_SOURCES
    arbiter.c
    debug.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/arb
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    _NTOS_
    _NTIFS_
    _NTDDK_
    BUFFER_OVERFLOW_CHECKS=0
)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror)
endif()

message(STATUS "Configured arb module: ${TARGET_NAME} with 2 sources")
