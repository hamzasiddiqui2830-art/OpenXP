# perf Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME perf)
set(TARGET_NAME ntosperf)

set(MODULE_SOURCES
    hooks.c
    logging.c
    perfdata.c
    perfsup.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/perf
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

message(STATUS "Configured perf module: ${TARGET_NAME} with 4 sources")
