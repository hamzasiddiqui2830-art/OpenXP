# raw Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME raw)
set(TARGET_NAME ntosraw)

# Source files from sources.inc
set(MODULE_SOURCES
    Cleanup.c  
    ${REACTOS_SOURCE_DIR}/ntoskrnl/Close.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/Create.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FileInfo.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/FsCtrl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/StrucSup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/RawDisp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/RawInit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/ReadWrit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/VolInfo.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/raw
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

message(STATUS "Configured raw module: ${TARGET_NAME} with 10 sources")
