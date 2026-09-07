# verifier Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME verifier)
set(TARGET_NAME ntosverifier)

# Source files from sources.inc
set(MODULE_SOURCES
    ..vfbugcheck.c
    ..vfddi.c
    ..vfdeadlock.c
    ..vfdebug.c
    ..vfdevobj.c
    ..vffileio.c
    ..vffilter.c
    ..vfhal.c
    ..vfinit.c
    ..vfirp.c
    ..vfirpdb.c
    ..vfirplog.c
    ..vfgeneric.c
    ..vfmajor.c
    ..vfmessage.c
    ..vfpacket.c
    ..vfpnp.c
    ..vfpower.c
    ..vfprint.c
    ..vfrandom.c
    ..vfsettings.c
    ..vfstack.c
    ..vfutil.c
    ..vfwmi.c
    ..vfzwapi.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../ke
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

message(STATUS "Configured verifier module: ${TARGET_NAME} with 25 sources")
