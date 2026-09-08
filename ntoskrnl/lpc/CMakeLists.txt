# lpc Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME lpc)
set(TARGET_NAME ntoslpc)

# Source files from sources.inc
set(MODULE_SOURCES
    lpcinit.c  
    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpccreat.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpclistn.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcconn.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpccompl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcsend.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcrecv.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcreply.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcquery.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcqueue.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcclose.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpcpriv.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpclog.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/lpc
    ${REACTOS_SOURCE_DIR}/ntoskrnl
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

# Precompiled header configuration (optional)
# set_target_properties(${TARGET_NAME} PROPERTIES
#     COMPILE_PCH_OUTPUT_INIT "lpcp.obj"
# )

message(STATUS "Configured lpc module: ${TARGET_NAME} with 13 sources")
