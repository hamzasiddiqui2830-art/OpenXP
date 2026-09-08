# config Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME config)
set(TARGET_NAME ntosconfig)

# Source files from sources.inc
set(MODULE_SOURCES
    cmapi.c     
    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmapi2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmboot.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmchek.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmchek2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmclose.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmconfig.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmcontrl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdat.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdat2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdat3.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdatini.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdelete.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmgquota.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmhvlist.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmindex.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cminit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmname.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmnotify.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmparse.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmparse2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmquery.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsavres.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsubs.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsubs2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsubs3.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmse.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsysini.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmtrecpy.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmtredel.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmtree.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmworker.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmwrapr.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmwrapr2.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmwmi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmmapvw.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdelay.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmvalue.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmsecache.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmdown.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmalloc.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/cmhook.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivebin.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivecell.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivechek.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivefree.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hiveinit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hiveload.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivemap.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivesum.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivesync.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hivehint.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/ntapi.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hwprofil.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/config
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

# Tests are opt-in and are not configured or compiled unless explicitly enabled.
if(BUILD_NTOSKRNL_TESTS)
    add_subdirectory(test)
    message(STATUS "Configured config test targets")
endif()

message(STATUS "Configured config module: ${TARGET_NAME} with 54 sources")
