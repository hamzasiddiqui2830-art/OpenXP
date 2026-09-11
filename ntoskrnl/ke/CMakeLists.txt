# ke Module CMakeLists.txt

set(MODULE_NAME ke)
set(TARGET_NAME ntoske)

set(MODULE_SOURCES
    apcobj.c
    apcsup.c
    balmgr.c
    bugcheck.c
    config.c
    debug.c
    devquobj.c
    dpclock.c
    dpcobj.c
    dpcsup.c
    eventobj.c
    idsched.c
    interobj.c
    kernldat.c
    kevutil.c
    kiinit.c
    miscc.c
    mutntobj.c
    procobj.c
    profobj.c
    queueobj.c
    raisexcp.c
    semphobj.c
    thredobj.c
    thredsup.c
    timerobj.c
    timersup.c
    wait.c
    waitsup.c
    xipi.c
    yield.c
)

if(WRK_ARCH_NAME STREQUAL "i386")
    list(APPEND MODULE_SOURCES
        i386/abiosc.c
        i386/allproc.c
        i386/apcuser.c
        i386/biosc.c
        i386/callback.c
        i386/exceptn.c
        i386/flush.c
        i386/flushtb.c
        i386/gdtsup.c
        i386/i386init.c
        i386/intobj.c
        i386/iopm.c
        i386/kernlini.c
        i386/largepag.c
        i386/ldtsup.c
        i386/misc.c
        i386/mtrr.c
        i386/mtrramd.c
        i386/pat.c
        i386/thredini.c
        i386/trapc.c

    )
    list(APPEND MODULE_ASM_SOURCES
        i386/abiosa.asm
        i386/callout.asm
        i386/clockint.asm
        i386/cpu.asm
        i386/ctxswap.asm
        i386/emv86.asm
        i386/emxcptn.asm
        i386/i386pcr.asm
        i386/instemul.asm
        i386/int.asm
        i386/intsup.asm
        i386/ldtsup2.asm
        i386/mpipia.asm
        i386/newsysbg.asm
        i386/procstat.asm
        i386/spindbg.asm
        i386/spinlock.asm
        i386/sysstubs.asm
        i386/systable.asm
        i386/threadbg.asm
        i386/timindex.asm
        i386/trap.asm
        i386/zero.asm)
elseif(WRK_ARCH_NAME STREQUAL "amd64")
    list(APPEND MODULE_SOURCES
        amd64/allproc.c
        amd64/apcuser.c
        amd64/callback.c
        amd64/decode.c
        amd64/exceptn.c
        amd64/flush.c
        amd64/flushtb.c
        amd64/initkr.c
        amd64/intobj.c
        amd64/ipi.c
        amd64/misc.c
        amd64/pat.c
        amd64/queuelock.c
        amd64/runtime.c
        amd64/spinlock.c
        amd64/thredini.c
        )
    list(APPEND MODULE_ASM_SOURCES
        amd64/apcint.asm
        amd64/callout.asm
        amd64/clockint.asm
        amd64/cpuid.asm
        amd64/ctxswap.asm
        amd64/dpcint.asm
        amd64/idle.asm
        amd64/intipi.asm
        amd64/intsup.asm
        amd64/miscs.asm
        amd64/procstat.asm
        amd64/profint.asm
        amd64/start.asm
        amd64/sysstubs.asm
        amd64/systable.asm
        amd64/threadbg.asm
        amd64/trap.asm
        amd64/zero.asm
    )
endif()

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES} ${MODULE_ASM_SOURCES})

if(MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY ""
    )
endif()

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/ke
    ${REACTOS_SOURCE_DIR}/ntoskrnl
    ${REACTOS_SOURCE_DIR}/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/${WRK_ARCH_NAME}
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
    add_subdirectory(tests)
    message(STATUS "Configured KE test targets")
endif()

message(STATUS "Configured ke module: ${TARGET_NAME}")
