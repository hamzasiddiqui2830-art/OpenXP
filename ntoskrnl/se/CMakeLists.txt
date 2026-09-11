# Security module CMakeLists.txt
# The source list is explicit; do not glob this directory because it also
# contains legacy user-mode/test material.

set(MODULE_NAME se)
set(TARGET_NAME ntos_se)

set(MODULE_SOURCES
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/accessck.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/capture.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/privileg.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/rmlogon.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/rmmain.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/rmvars.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seassign.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seaudit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/sepaudit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seclient.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seglobal.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/semethod.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/sep.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/subject.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/seastate.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/token.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/tokenadj.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/tokendup.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/tokenopn.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/tokenqry.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/tokenset.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/adtlog.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/adtinit.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/adtvars.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/adtutil.c
    ${REACTOS_SOURCE_DIR}/ntoskrnl/se/rmaudit.c
)

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

message(STATUS "Configured se module: ${TARGET_NAME}")
