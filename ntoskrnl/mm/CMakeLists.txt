# mm Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME mm)
set(TARGET_NAME ntosmm)

# Source files from sources.inc
set(MODULE_SOURCES
    acceschk.c 
    ..addrsup.c
    ..allocpag.c
    ..allocvm.c
    ..buildmdl.c
    ..checkpfn.c
    ..checkpte.c
    ..compress.c
    ..crashdmp.c
    ..creasect.c
    ..debugsup.c
    ..deleteva.c
    ..dmpaddr.c
    ..dynmem.c
    ..extsect.c
    ..flushbuf.c
    ..flushsec.c
    ..forksup.c
    ..freevm.c
    ..hypermap.c
    ..iosup.c
    ..lockvm.c
    ..mapcache.c
    ..mapview.c
    ..miglobal.c
    ..mirror.c
    ..mmfault.c
    ..mminit.c
    ..mmsup.c
    ..mmpatch.c
    ..mmquota.c
    ..modwrite.c
    ..nolowmem.c
    ..pagfault.c
    ..pfndec.c
    ..pfnlist.c
    ..pfsup.c
    ..physical.c
    ..procsup.c
    ..protect.c
    ..querysec.c
    ..queryvm.c
    ..readwrt.c
    ..sectsup.c
    ..session.c
    ..sessload.c
    ..shutdown.c
    ..specpool.c
    ..sysload.c
    ..sysptes.c
    ..triage.c
    ..umapview.c
    ..vadtree.c
    ..verifier.c
    ..wslist.c
    ..wsmanage.c
    ..wstree.c
    ..wrtfault.c
    ..wrtwatch.c
    ..zeropage.c
)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ke
    ${CMAKE_CURRENT_SOURCE_DIR}/../../kd64
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
#     COMPILE_PCH_OUTPUT_INIT "mi.obj"
# )

message(STATUS "Configured mm module: ${TARGET_NAME} with 60 sources")
