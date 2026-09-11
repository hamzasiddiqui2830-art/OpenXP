# ke Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ke)
set(TARGET_NAME ntoske)

set(MODULE_SOURCES
    apcobj.c
    apcsup.c
    balmgr.c
    bugcheck.c
    config.c
    debug.c
    devquobj.c
    dpclock.c
    dpcobj.c
    dpcsup.c
    eventobj.c
    idsched.c
    interobj.c
    kernldat.c
    kevutil.c
    kiinit.c
    miscc.c
    mutntobj.c
    procobj.c
    profobj.c
    queueobj.c
    raisexcp.c
    semphobj.c
    thredobj.c
    thredsup.c
    timerobj.c
    timersup.c
    wait.c
    waitsup.c
    xipi.c
    yield.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/${WRK_ARCH_NAME}
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

if(BUILD_NTOSKRNL_TESTS)
    add_subdirectory(tests)
    message(STATUS "Configured KE test targets")
endif()

message(STATUS "Configured ke module: ${TARGET_NAME} with 31 sources")
