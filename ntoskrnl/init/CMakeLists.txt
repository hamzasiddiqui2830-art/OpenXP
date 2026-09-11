# init Module CMakeLists.txt
# Explicit source manifest for the kernel initialization module.

set(MODULE_NAME init)
set(TARGET_NAME ntosinit)

set(MODULE_SOURCES
    anim.c
    bootvid.c
    hello.c
    init.c
    initos.c
    ntkrnlmp.c
    ntkrnlpa.c
    ntkrpamp.c
    ntoskrnl.c
    port.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/init
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/ntoskrnl
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

message(STATUS "Configured init module: ${TARGET_NAME} with 10 sources")
