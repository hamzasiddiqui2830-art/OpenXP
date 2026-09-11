# po Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME po)
set(TARGET_NAME ntospo)

set(MODULE_SOURCES
    attrib.c
    paction.c
    pbatt.c
    pidle.c
    pinfo.c
    poinit.c
    podata.c
    postate.c
    pocall.c
    poshtdwn.c
    pwork.c
    switch.c
    notify.c
    hiber.c
    idle.c
    misc.c
    ntapi.c
    sidle.c
    suspend.c
    sys.c
    sysdev.c
    thermal.c
    throttle.c
    volume.c
)

add_library(${TARGET_NAME} STATIC ${MODULE_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/po
    ${REACTOS_SOURCE_DIR}/inc
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
    message(STATUS "Configured PO test targets")
endif()

message(STATUS "Configured po module: ${TARGET_NAME} with 24 sources")
