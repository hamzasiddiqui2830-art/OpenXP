# po Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME po)
set(TARGET_NAME ntospo)

# Source files from sources.inc
set(MODULE_SOURCES
    attrib.c     
    ${REACTOS_SOURCE_DIR}/ntoskrnl/paction.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pbatt.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pidle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pinfo.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/poinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/podata.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/postate.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pocall.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/poshtdwn.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pwork.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/switch.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/notify.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hiber.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/idle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/misc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/ntapi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sidle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/suspend.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sys.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sysdev.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/thermal.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/throttle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/volume.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/po
    ${REACTOS_SOURCE_DIR}/inc
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

# Include tests subdirectory for separate test targets
add_subdirectory(tests)

message(STATUS "Configured po module: ${TARGET_NAME} with 24 sources")
