# cache Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME cache)
set(TARGET_NAME ntoscache)

# Source files from sources.inc
set(MODULE_SOURCES
    cachedat.c   
    ..cachesub.c
    ..copysup.c
    ..fssup.c
    ..lazyrite.c
    ..logsup.c
    ..mdlsup.c
    ..pinsup.c
    ..prefboot.c
    ..prefetch.c
    ..prefparm.c
    ..ccperf.c
    ..vacbsup.c
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
#     COMPILE_PCH_OUTPUT_INIT "cc.obj"
# )

message(STATUS "Configured cache module: ${TARGET_NAME} with 13 sources")
