# io Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME io)
set(TARGET_NAME ntosio)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ioguid.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/iovutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/netboot.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sessnirp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/trackirp.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr
    ${REACTOS_SOURCE_DIR}/ntoskrnl/pnp
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
#     COMPILE_PCH_OUTPUT_INIT "iop.obj"
# )

message(STATUS "Configured io module: ${TARGET_NAME} with 5 sources")
