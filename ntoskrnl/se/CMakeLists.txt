# se Module CMakeLists.txt
# Converted from legacy sources.inc

set(MODULE_NAME se)
set(TARGET_NAME ntos_se)

# Explicit kernel Security sources. Do not glob this directory: it also
# contains legacy user-mode/test programs that are not part of ntos_se.
set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/accessck.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/capture.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/privileg.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmlogon.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmmain.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmvars.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seassign.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seaudit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/sepaudit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seclient.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seglobal.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/semethod.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/sep.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/subject.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/seastate.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/token.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenadj.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokendup.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenopn.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenqry.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/tokenset.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtlog.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtvars.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/adtutil.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rmaudit.c
)

add_wrk_module(se
    TARGET_NAME ${TARGET_NAME}
    SOURCE_DIR ${REACTOS_SOURCE_DIR}/ntoskrnl/se
    SOURCES ${MODULE_SOURCES}
)

message(STATUS "Configured se module: ${TARGET_NAME} with ${CMAKE_MATCH_COUNT} explicit sources")
