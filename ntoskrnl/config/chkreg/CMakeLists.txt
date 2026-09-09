# Legacy ntos-old/config/chkreg/sources target.
# This is a user-mode console PROGRAM, not part of ntosconfig.

set(CHKREG_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/regcell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reglist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/regbin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/regdmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/regsav.c
    ${CMAKE_CURRENT_SOURCE_DIR}/regmain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/reglog.c
)

add_executable(chkreg ${CHKREG_SOURCES})

# Legacy chkreg uses the user-mode Windows headers and the MS C runtime.
# Keep those requirements isolated from the kernel targets.
target_include_directories(chkreg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ntoskrnl/config
    ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
    ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/base/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/sdktools/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
    ${CMAKE_SOURCE_DIR}/sdk/halkit/inc
    ${CMAKE_SOURCE_DIR}/base/inc
)

target_compile_definitions(chkreg PRIVATE
    WIN32
    _WINDOWS
)

if(MSVC)
    target_compile_options(chkreg PRIVATE /W3 /WX /MD)
endif()

set_target_properties(chkreg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
