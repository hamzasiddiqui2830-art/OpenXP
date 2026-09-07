# se Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME se)
set(TARGET_NAME ntosse)

# Source files from sources.inc
set(MODULE_SOURCES
    ..accessck.c
    ..capture.c
    ..privileg.c
    ..rmlogon.c
    ..rmmain.c
    ..rmvars.c
    ..seassign.c
    ..seaudit.c
    ..sepaudit.c
    ..seclient.c
    ..seglobal.c
    ..seinit.c
    ..semethod.c
    ..sep.c
    ..subject.c
    ..seastate.c
    ..token.c
    ..tokenadj.c
    ..tokendup.c
    ..tokenopn.c
    ..tokenqry.c
    ..tokenset.c
    ..adtlog.c
    ..adtinit.c
    ..adtvars.c
    ..adtutil.c
    ..rmaudit.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT ""
# )

message(STATUS "Configured se module: ${TARGET_NAME} with 27 sources")
