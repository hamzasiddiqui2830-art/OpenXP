# io Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME io)
set(TARGET_NAME ntosio)

set(MODULE_SOURCES
    ioguid.c
    iovutil.c
    netboot.c
    sessnirp.c
    trackirp.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl/io/pnpmgr
    ${REACTOS_SOURCE_DIR}/ntoskrnl/pnp
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

message(STATUS "Configured io module: ${TARGET_NAME} with 5 sources")
