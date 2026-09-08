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

# Build each test as a separate executable
add_executable(rtl_test_array array.c)
target_include_directories(rtl_test_array PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_array PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_recip recip.c)
target_include_directories(rtl_test_recip PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_recip PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_tacl tacl.c)
target_include_directories(rtl_test_tacl PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_tacl PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_tbitmap tbitmap.c)
target_include_directories(rtl_test_tbitmap PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_tbitmap PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_tnlsxlat tnlsxlat.c)
target_include_directories(rtl_test_tnlsxlat PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_tnlsxlat PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_toverflow toverflow.c)
target_include_directories(rtl_test_toverflow PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_toverflow PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_tprefix tprefix.c)
target_include_directories(rtl_test_tprefix PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_tprefix PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_trandom trandom.c)
target_include_directories(rtl_test_trandom PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_trandom PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_trtl trtl.c)
target_include_directories(rtl_test_trtl PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_trtl PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_tsplay tsplay.c)
target_include_directories(rtl_test_tsplay PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_tsplay PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_ttime ttime.c)
target_include_directories(rtl_test_ttime PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_ttime PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_ttri ttri.c)
target_include_directories(rtl_test_ttri PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_ttri PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_ucli ucli.c)
target_include_directories(rtl_test_ucli PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_ucli PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_uexec uexec.c)
target_include_directories(rtl_test_uexec PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_uexec PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_uexec1 uexec1.c)
target_include_directories(rtl_test_uexec1 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_uexec1 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_uexec2 uexec2.c)
target_include_directories(rtl_test_uexec2 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_uexec2 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_urtl urtl.c)
target_include_directories(rtl_test_urtl PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_urtl PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_utxcpt1 utxcpt1.c)
target_include_directories(rtl_test_utxcpt1 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_utxcpt1 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_utxcpt2 utxcpt2.c)
target_include_directories(rtl_test_utxcpt2 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_utxcpt2 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_utxcpt3 utxcpt3.c)
target_include_directories(rtl_test_utxcpt3 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_utxcpt3 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(rtl_test_utxcpt4 utxcpt4.c)
target_include_directories(rtl_test_utxcpt4 PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/rtl
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
)
target_compile_definitions(rtl_test_utxcpt4 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

message(STATUS "Configured RTL test targets as separate executables")
