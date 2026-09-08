# RTL Test Programs CMakeLists.txt
# Each test is built as a separate executable target

set(RTL_TEST_SOURCES
    array.c
    recip.c
    tacl.c
    tbitmap.c
    tnlsxlat.c
    toverflow.c
    tprefix.c
    trandom.c
    trtl.c
    tsplay.c
    ttime.c
    ttri.c
    ucli.c
    uexec.c
    uexec1.c
    uexec2.c
    urtl.c
    utxcpt1.c
    utxcpt2.c
    utxcpt3.c
    utxcpt4.c
)

# The WRK sources use the compiler-independent spellings try/except/finally
# and lowercase exception helper names. MSVC C exposes these as __try/__except/
# __finally and GetExceptionCode/AbnormalTermination. Keep the compatibility
# mapping local to the test targets instead of changing kernel headers globally.
set(RTL_TEST_COMPAT_DEFINES)
if(MSVC)
    list(APPEND RTL_TEST_COMPAT_DEFINES
        "try=__try"
        "except=__except"
        "finally=__finally"
        "abnormal_termination()=AbnormalTermination()"
        "exception_code()=GetExceptionCode()"
    )
endif()

# Build each test as a separate executable
foreach(TEST_SOURCE IN LISTS RTL_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)
    add_executable(rtl_test_${TEST_NAME} "${TEST_SOURCE}")
    target_include_directories(rtl_test_${TEST_NAME} PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    )
    target_compile_definitions(rtl_test_${TEST_NAME} PRIVATE
        _NTOS_ _NTIFS_ _NTDDK_
        ${RTL_TEST_COMPAT_DEFINES}
    )
endforeach()

message(STATUS "Configured RTL test targets as separate executables")
