# ob Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME ob)
set(TARGET_NAME ntosob)

# Source files from sources.inc
set(MODULE_SOURCES
    obinit.c   
    ${REACTOS_SOURCE_DIR}/ntoskrnl/obcreate.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obhandle.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obinsert.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obperf.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obref.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obse.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obtype.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obdir.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obdevmap.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/oblink.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obclose.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obquery.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obsdata.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obwait.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/obvutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/fastref.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ob
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
#     COMPILE_PCH_OUTPUT_INIT "obp.obj"
# )

message(STATUS "Configured ob module: ${TARGET_NAME} with 17 sources")
