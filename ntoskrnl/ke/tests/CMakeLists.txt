# KE Tests CMakeLists.txt
# Each test is built as a separate executable target

# npxstres test - requires i386 specific files
# Note: This test has architecture-specific sources that may not be available
if(EXISTS "${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/npxstres/i386/npxstres.c")
    add_executable(ke_test_npxstres
        npxstres/i386/npxstres.c
        npxstres/i386/fpexception.c
        npxstres/i386/kmode.c
    )
    target_include_directories(ke_test_npxstres PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/npxstres
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    )
    target_compile_definitions(ke_test_npxstres PRIVATE _NTOS_ _NTIFS_ _NTDDK_)
endif()

# x86div test - requires i386 specific files
if(EXISTS "${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/x86div/i386/test.c")
    add_executable(ke_test_x86div
        x86div/i386/test.c
        x86div/i386/testa.asm
    )
    target_include_directories(ke_test_x86div PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/x86div
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    )
    target_compile_definitions(ke_test_x86div PRIVATE _NTOS_ _NTIFS_ _NTDDK_)
endif()

# xcphnd test
if(EXISTS "${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/xcphnd/xcpt4.c")
    add_executable(ke_test_xcphnd
        xcphnd/xcpt4.c
    )
    target_include_directories(ke_test_xcphnd PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/ke/tests/xcphnd
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    )
    target_compile_definitions(ke_test_xcphnd PRIVATE _NTOS_ _NTIFS_ _NTDDK_)
endif()

message(STATUS "Configured KE test targets as separate executables")
