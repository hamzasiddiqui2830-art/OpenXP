# ke Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ke)
set(TARGET_NAME ntoske)

# Source files from sources.inc
set(MODULE_SOURCES
    ..apcobj.c
    ..apcsup.c
    ..balmgr.c
    ..bugcheck.c
    ..config.c
    ..debug.c
    ..devquobj.c
    ..dpclock.c
    ..dpcobj.c
    ..dpcsup.c
    ..eventobj.c
    ..idsched.c
    ..interobj.c
    ..kernldat.c
    ..kevutil.c
    ..kiinit.c
    ..miscc.c
    ..mutntobj.c
    ..procobj.c
    ..profobj.c
    ..queueobj.c
    ..raisexcp.c
    ..semphobj.c
    ..thredobj.c
    ..thredsup.c
    ..timerobj.c
    ..timersup.c
    ..wait.c
    ..waitsup.c
    ..xipi.c
    ..yield.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../$(TARGET_DIRECTORY)
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT "ki.obj"
# )

# Include tests subdirectory for separate test targets
add_subdirectory(tests)

message(STATUS "Configured ke module: ${TARGET_NAME} with 31 sources")
