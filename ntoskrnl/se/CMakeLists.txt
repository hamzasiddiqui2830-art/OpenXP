# se Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME se)
set(TARGET_NAME ntosse)

# Source files from sources.inc
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/accessck.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/capture.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/privileg.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmlogon.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmmain.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmvars.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seassign.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seaudit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sepaudit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seclient.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seglobal.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/semethod.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sep.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/subject.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/seastate.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/token.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenadj.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokendup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenopn.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenqry.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenset.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtlog.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtvars.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtutil.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmaudit.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se
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
#     COMPILE_PCH_OUTPUT_INIT ""
# )

message(STATUS "Configured se module: ${TARGET_NAME} with 27 sources")
