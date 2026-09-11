# cache Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME cache)
set(TARGET_NAME ntoscache)

set(MODULE_SOURCES
    cachedat.c
    cachesub.c
    copysup.c
    fssup.c
    lazyrite.c
    logsup.c
    mdlsup.c
    pinsup.c
    prefboot.c
    prefetch.c
    prefparm.c
    ccperf.c
    vacbsup.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/cache
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

message(STATUS "Configured cache module: ${TARGET_NAME} with 13 sources")
