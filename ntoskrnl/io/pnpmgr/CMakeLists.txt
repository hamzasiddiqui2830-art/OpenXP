# pnpmgr Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME pnpmgr)
set(TARGET_NAME ntospnpmgr)

# Source files from sources.inc
set(MODULE_SOURCES
    ..assign.c
    ..devices.c
    ..devnode.c
    ..devintrf.c
    ..dockhwp.c
    ..iofileutil.c
    ..mapper.c
    ..notify.c
    ..pnpbusno.c
    ..pnpcvrt.c
    ..pnpdata.c
    ..pnpdd.c
    ..pnpdel.c
    ..pnpdma.c
    ..pnpeisa.c
    ..pnpenum.c
    ..pnpevent.c
    ..pnpinit.c
    ..pnpioapi.c
    ..pnpirp.c
    ..pnpirq.c
    ..pnpmap.c
    ..pnpmemio.c
    ..pnppower.c
    ..pnpres.c
    ..pnprlist.c
    ..pnpstart.c
    ..pnpsubs.c
    ..ppcddb.c
    ..ppcontrol.c
    ..pphandle.c
    ..pphotswap.c
    ..pplastgood.c
    ..pppagepath.c
    ..ppdrvdb.c
    ..ppprofile.c
    ..ppvutil.c
    ..remlock.c
    ..report.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../pnp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
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
#     COMPILE_PCH_OUTPUT_INIT "pnpmgrp.obj"
# )

message(STATUS "Configured pnpmgr module: ${TARGET_NAME} with 39 sources")
