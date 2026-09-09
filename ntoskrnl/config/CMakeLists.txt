# config Module CMakeLists.txt
# Exact kernel source set from ntoskrnl/config/sources.inc.
# The legacy chkreg utility is a separate user-mode target under chkreg/.

set(MODULE_NAME config)
set(TARGET_NAME ntosconfig)

# sources.inc lists files relative to ntoskrnl/config. Keep that directory
# as the source root instead of looking for these files directly under
# ntoskrnl/.
set(CONFIG_SOURCE_DIR ${REACTOS_SOURCE_DIR}/ntoskrnl/config)

set(MODULE_SOURCES
    ${CONFIG_SOURCE_DIR}/cmapi.c
    ${CONFIG_SOURCE_DIR}/cmapi2.c
    ${CONFIG_SOURCE_DIR}/cmboot.c
    ${CONFIG_SOURCE_DIR}/cmchek.c
    ${CONFIG_SOURCE_DIR}/cmchek2.c
    ${CONFIG_SOURCE_DIR}/cmclose.c
    ${CONFIG_SOURCE_DIR}/cmconfig.c
    ${CONFIG_SOURCE_DIR}/cmcontrl.c
    ${CONFIG_SOURCE_DIR}/cmdat.c
    ${CONFIG_SOURCE_DIR}/cmdat2.c
    ${CONFIG_SOURCE_DIR}/cmdat3.c
    ${CONFIG_SOURCE_DIR}/cmdatini.c
    ${CONFIG_SOURCE_DIR}/cmdelete.c
    ${CONFIG_SOURCE_DIR}/cmgquota.c
    ${CONFIG_SOURCE_DIR}/cmhvlist.c
    ${CONFIG_SOURCE_DIR}/cmindex.c
    ${CONFIG_SOURCE_DIR}/cminit.c
    ${CONFIG_SOURCE_DIR}/cmname.c
    ${CONFIG_SOURCE_DIR}/cmnotify.c
    ${CONFIG_SOURCE_DIR}/cmparse.c
    ${CONFIG_SOURCE_DIR}/cmparse2.c
    ${CONFIG_SOURCE_DIR}/cmquery.c
    ${CONFIG_SOURCE_DIR}/cmsavres.c
    ${CONFIG_SOURCE_DIR}/cmsubs.c
    ${CONFIG_SOURCE_DIR}/cmsubs2.c
    ${CONFIG_SOURCE_DIR}/cmsubs3.c
    ${CONFIG_SOURCE_DIR}/cmse.c
    ${CONFIG_SOURCE_DIR}/cmsysini.c
    ${CONFIG_SOURCE_DIR}/cmtrecpy.c
    ${CONFIG_SOURCE_DIR}/cmtredel.c
    ${CONFIG_SOURCE_DIR}/cmtree.c
    ${CONFIG_SOURCE_DIR}/cmworker.c
    ${CONFIG_SOURCE_DIR}/cmwrapr.c
    ${CONFIG_SOURCE_DIR}/cmwrapr2.c
    ${CONFIG_SOURCE_DIR}/cmwmi.c
    ${CONFIG_SOURCE_DIR}/cmmapvw.c
    ${CONFIG_SOURCE_DIR}/cmdelay.c
    ${CONFIG_SOURCE_DIR}/cmvalue.c
    ${CONFIG_SOURCE_DIR}/cmsecache.c
    ${CONFIG_SOURCE_DIR}/cmdown.c
    ${CONFIG_SOURCE_DIR}/cmalloc.c
    ${CONFIG_SOURCE_DIR}/cmhook.c
    ${CONFIG_SOURCE_DIR}/hivebin.c
    ${CONFIG_SOURCE_DIR}/hivecell.c
    ${CONFIG_SOURCE_DIR}/hivechek.c
    ${CONFIG_SOURCE_DIR}/hivefree.c
    ${CONFIG_SOURCE_DIR}/hiveinit.c
    ${CONFIG_SOURCE_DIR}/hiveload.c
    ${CONFIG_SOURCE_DIR}/hivemap.c
    ${CONFIG_SOURCE_DIR}/hivesum.c
    ${CONFIG_SOURCE_DIR}/hivesync.c
    ${CONFIG_SOURCE_DIR}/hivehint.c
    ${CONFIG_SOURCE_DIR}/ntapi.c
    ${CONFIG_SOURCE_DIR}/hwprofil.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/config
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    _NTOS_
    _NTIFS_
    _NTDDK_
)

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror)
endif()

if(BUILD_NTOSKRNL_TESTS)
    add_subdirectory(test)
    message(STATUS "Configured config test targets")
endif()

message(STATUS "Configured config module: ${TARGET_NAME} with ${CMAKE_CURRENT_LIST_FILE} source set")
