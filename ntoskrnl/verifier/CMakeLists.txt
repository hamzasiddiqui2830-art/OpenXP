# verifier Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME verifier)
set(TARGET_NAME ntosverifier)

set(MODULE_SOURCES
    vfbugcheck.c
    vfddi.c
    vfdeadlock.c
    vfdebug.c
    vfdevobj.c
    vffileio.c
    vffilter.c
    vfhal.c
    vfinit.c
    vfirp.c
    vfirpdb.c
    vfirplog.c
    vfgeneric.c
    vfmajor.c
    vfmessage.c
    vfpacket.c
    vfpnp.c
    vfpower.c
    vfprint.c
    vfrandom.c
    vfsettings.c
    vfstack.c
    vfutil.c
    vfwmi.c
    vfzwapi.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/verifier
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
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

message(STATUS "Configured verifier module: ${TARGET_NAME} with 25 sources")
