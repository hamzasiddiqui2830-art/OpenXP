# wmi Module CMakeLists.txt
# Explicit source manifest for the WMI kernel module.
# Sample and KD extension sources are intentionally listed explicitly.

set(MODULE_NAME wmi)
set(TARGET_NAME ntoswmi)

set(MODULE_SOURCES
    alloc.c
    api.c
    callouts.c
    chunk.c
    consumer.c
    dataprov.c
    diags.c
    ds.c
    enabdisa.c
    globalog.c
    kdexts/tracing/kdexts.c
    kdexts/tracing/kdextsin.c
    kdexts/tracing/wmitrace.c
    mca.c
    notify.c
    provider.c
    register.c
    sample/wmifilt/filter.c
    sample/wmifilt/pnp.c
    sample/wmifilt/power.c
    sample/wmifilt/util.c
    sample/wmifilt/wmisamp.c
    sample/wmilib/wmisamp.c
    secure.c
    smbios.c
    traceapi.c
    tracelog.c
    tracesup.c
    wmi.c
    wmidata.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/wmi
    ${REACTOS_SOURCE_DIR}/ntoskrnl/wmi/kdexts
    ${REACTOS_SOURCE_DIR}/ntoskrnl/wmi/kdexts/tracing
    ${REACTOS_SOURCE_DIR}/ntoskrnl/wmi/sample/wmifilt
    ${REACTOS_SOURCE_DIR}/ntoskrnl/wmi/sample/wmilib
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

message(STATUS "Configured wmi module: ${TARGET_NAME} with 30 sources")
