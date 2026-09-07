# po Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME po)
set(TARGET_NAME ntospo)

# Source files from sources.inc
set(MODULE_SOURCES
    attrib.c     
    ..paction.c
    ..pbatt.c
    ..pidle.c
    ..pinfo.c
    ..poinit.c
    ..podata.c
    ..postate.c
    ..pocall.c
    ..poshtdwn.c
    ..pwork.c
    ..switch.c
    ..notify.c
    ..hiber.c
    ..idle.c
    ..misc.c
    ..ntapi.c
    ..sidle.c
    ..suspend.c
    ..sys.c
    ..sysdev.c
    ..thermal.c
    ..throttle.c
    ..volume.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
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

message(STATUS "Configured po module: ${TARGET_NAME} with 24 sources")
