# mm Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME mm)
set(TARGET_NAME ntosmm)

# Source files from sources.inc
set(MODULE_SOURCES
    acceschk.c 
    ${REACTOS_SOURCE_DIR}/ntoskrnl/addrsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/allocpag.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/allocvm.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/buildmdl.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/checkpfn.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/checkpte.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/compress.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/crashdmp.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/creasect.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/debugsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/deleteva.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/dmpaddr.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/dynmem.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/extsect.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/flushbuf.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/flushsec.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/forksup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/freevm.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/hypermap.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/iosup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/lockvm.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mapcache.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mapview.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/miglobal.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mirror.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mmfault.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mminit.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mmsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mmpatch.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/mmquota.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/modwrite.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/nolowmem.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pagfault.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pfndec.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pfnlist.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/pfsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/physical.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/procsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/protect.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/querysec.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/queryvm.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/readwrt.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sectsup.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/session.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sessload.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/shutdown.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/specpool.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sysload.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/sysptes.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/triage.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/umapview.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/vadtree.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/verifier.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wslist.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wsmanage.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wstree.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wrtfault.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/wrtwatch.c

    ${REACTOS_SOURCE_DIR}/ntoskrnl/zeropage.c

)

# Create library target
add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

# Set include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/mm
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ke
    ${REACTOS_SOURCE_DIR}/kd64
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
#     COMPILE_PCH_OUTPUT_INIT "mi.obj"
# )

message(STATUS "Configured mm module: ${TARGET_NAME} with 60 sources")
