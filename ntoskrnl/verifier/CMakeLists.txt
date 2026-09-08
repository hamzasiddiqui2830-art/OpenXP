# verifier Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME verifier)
set(TARGET_NAME ntosverifier)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfbugcheck.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfddi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfdeadlock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfdebug.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfdevobj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vffileio.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vffilter.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfhal.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfirp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfirpdb.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfirplog.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfgeneric.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfmajor.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfmessage.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfpacket.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfpnp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfpower.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfprint.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfrandom.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfsettings.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfstack.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfwmi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vfzwapi.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/verifier
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
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

message(STATUS "Configured verifier module: ${TARGET_NAME} with 25 sources")
