# ke Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ke)
set(TARGET_NAME ntoske)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/apcobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/apcsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/balmgr.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/bugcheck.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/config.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/debug.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/devquobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/dpclock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/dpcobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/dpcsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/eventobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/idsched.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/interobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/kernldat.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/kevutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/kiinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/miscc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mutntobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/procobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/profobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/queueobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/raisexcp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/semphobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/thredobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/thredsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/timerobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/timersup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wait.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/waitsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/xipi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/yield.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/${WRK_ARCH_NAME}
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)

# Compile definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    _NTOS_
    _NTIFS_
    _NTDDK_
)

# Compiler options - match legacy build settings (/W4 /WX = all warnings + warnings as errors)
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror)
endif()

# Tests are opt-in and are not configured or compiled unless explicitly enabled.
if(BUILD_NTOSKRNL_TESTS)
    add_subdirectory(tests)
    message(STATUS "Configured KE test targets")
endif()

message(STATUS "Configured ke module: ${TARGET_NAME} with 31 sources")
