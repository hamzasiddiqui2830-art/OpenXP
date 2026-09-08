# config Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME config)
set(TARGET_NAME ntosconfig)

# Source files from sources.inc
set(MODULE_SOURCES
    cmapi.c     
    ..cmapi2.c
    ..cmboot.c
    ..cmchek.c
    ..cmchek2.c
    ..cmclose.c
    ..cmconfig.c
    ..cmcontrl.c
    ..cmdat.c
    ..cmdat2.c
    ..cmdat3.c
    ..cmdatini.c
    ..cmdelete.c
    ..cmgquota.c
    ..cmhvlist.c
    ..cmindex.c
    ..cminit.c
    ..cmname.c
    ..cmnotify.c
    ..cmparse.c
    ..cmparse2.c
    ..cmquery.c
    ..cmsavres.c
    ..cmsubs.c
    ..cmsubs2.c
    ..cmsubs3.c
    ..cmse.c
    ..cmsysini.c
    ..cmtrecpy.c
    ..cmtredel.c
    ..cmtree.c
    ..cmworker.c
    ..cmwrapr.c
    ..cmwrapr2.c
    ..cmwmi.c
    ..cmmapvw.c
    ..cmdelay.c
    ..cmvalue.c
    ..cmsecache.c
    ..cmdown.c
    ..cmalloc.c
    ..cmhook.c
    ..hivebin.c
    ..hivecell.c
    ..hivechek.c
    ..hivefree.c
    ..hiveinit.c
    ..hiveload.c
    ..hivemap.c
    ..hivesum.c
    ..hivesync.c
    ..hivehint.c
    ..ntapi.c
    ..hwprofil.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ke
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
#     COMPILE_PCH_OUTPUT_INIT "cmp.obj"
# )

# Include test subdirectory for separate test targets
add_subdirectory(test)

message(STATUS "Configured config module: ${TARGET_NAME} with 54 sources")
