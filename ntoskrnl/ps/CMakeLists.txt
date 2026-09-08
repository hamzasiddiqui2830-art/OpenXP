# ps Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ps)
set(TARGET_NAME ntosps)

# Source files from sources.inc
set(MODULE_SOURCES
    psspnd.c   
    ${REACTOS_SOURCE_DIR}/ntoskrnl/pscid.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psimpers.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psdelete.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psquery.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/create.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/kulookup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psquota.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psctx.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psopen.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psjob.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/security.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pshelper.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/psenum.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ps
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ke
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
#     COMPILE_PCH_OUTPUT_INIT "psp.obj"
# )

message(STATUS "Configured ps module: ${TARGET_NAME} with 15 sources")
