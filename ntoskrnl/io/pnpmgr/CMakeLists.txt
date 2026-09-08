# pnpmgr Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME pnpmgr)
set(TARGET_NAME ntospnpmgr)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/assign.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/devices.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/devnode.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/devintrf.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/dockhwp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/iofileutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/mapper.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/notify.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpbusno.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpcvrt.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpdata.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpdd.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpdel.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpdma.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpeisa.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpenum.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpevent.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpioapi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpirp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpirq.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpmap.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpmemio.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnppower.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpres.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnprlist.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpstart.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpsubs.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ppcddb.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ppcontrol.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pphandle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pphotswap.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pplastgood.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pppagepath.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ppdrvdb.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ppprofile.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/ppvutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/remlock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/report.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/pnp
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT "pnpmgrp.obj"
# )

message(STATUS "Configured pnpmgr module: ${TARGET_NAME} with 39 sources")
