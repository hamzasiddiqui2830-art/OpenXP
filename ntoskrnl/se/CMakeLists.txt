# Security module CMakeLists.txt
# The source list is explicit; do not glob this directory because it also
# contains legacy user-mode/test material.

set(MODULE_NAME se)
set(TARGET_NAME ntos_se)

set(MODULE_SOURCES
    accessck.c
    capture.c
    privileg.c
    rmlogon.c
    rmmain.c
    rmvars.c
    seassign.c
    seaudit.c
    sepaudit.c
    seclient.c
    seglobal.c
    seinit.c
    semethod.c
    sep.c
    subject.c
    seastate.c
    token.c
    tokenadj.c
    tokendup.c
    tokenopn.c
    tokenqry.c
    tokenset.c
    adtlog.c
    adtinit.c
    adtvars.c
    adtutil.c
    rmaudit.c
)

add_wrk_module(${MODULE_NAME}
    TARGET_NAME ${TARGET_NAME}
    SOURCES ${MODULE_SOURCES}
)

message(STATUS "Configured se module: ${TARGET_NAME}")
