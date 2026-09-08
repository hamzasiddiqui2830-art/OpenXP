# FsRtl Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME fsrtl)
set(TARGET_NAME ntosFsRtl)

# Source files from sources.inc
set(MODULE_SOURCES
    DbcsName.c 
    ${REACTOS_SOURCE_DIR}/ntoskrnl/FastIo.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FaultTol.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FileLock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Filter.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FiltrCtx.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FsFilter.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FsRtlPc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/LargeMcb.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Name.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Notify.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Oplock.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/PnP.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Unc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Tunnel.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/StackOvf.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/fsrtl
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
#     COMPILE_PCH_OUTPUT_INIT "fsrtlp.obj"
# )

message(STATUS "Configured FsRtl module: ${TARGET_NAME} with 16 sources")
