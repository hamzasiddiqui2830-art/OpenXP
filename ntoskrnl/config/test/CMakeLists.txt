# Config Test Programs CMakeLists.txt
# Each test is built as a separate executable target

# Build each config test as a separate executable
add_executable(config_test_chkreg chkreg.c)
target_include_directories(config_test_chkreg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_chkreg PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_editreg editreg.c)
target_include_directories(config_test_editreg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_editreg PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_init386 init386.c)
target_include_directories(config_test_init386 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_init386 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_initdat initdat.c)
target_include_directories(config_test_initdat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_initdat PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_perform perform.c)
target_include_directories(config_test_perform PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_perform PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_regext regext.c)
target_include_directories(config_test_regext PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_regext PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtbatcr rtbatcr.c)
target_include_directories(config_test_rtbatcr PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtbatcr PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdelkey rtdelkey.c)
target_include_directories(config_test_rtdelkey PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdelkey PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdeltre rtdeltre.c)
target_include_directories(config_test_rtdeltre PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdeltre PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdelval rtdelval.c)
target_include_directories(config_test_rtdelval PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdelval PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdmp rtdmp.c)
target_include_directories(config_test_rtdmp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdmp PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdmpb rtdmpb.c)
target_include_directories(config_test_rtdmpb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdmpb PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdmpsec rtdmpsec.c)
target_include_directories(config_test_rtdmpsec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdmpsec PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtdmpval rtdmpval.c)
target_include_directories(config_test_rtdmpval PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtdmpval PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtflush rtflush.c)
target_include_directories(config_test_rtflush PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtflush PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtload rtload.c)
target_include_directories(config_test_rtload PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtload PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtmisc1 rtmisc1.c)
target_include_directories(config_test_rtmisc1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtmisc1 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtnotify rtnotify.c)
target_include_directories(config_test_rtnotify PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtnotify PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtqkey rtqkey.c)
target_include_directories(config_test_rtqkey PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtqkey PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtqval rtqval.c)
target_include_directories(config_test_rtqval PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtqval PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtrenval rtrenval.c)
target_include_directories(config_test_rtrenval PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtrenval PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtreplac rtreplac.c)
target_include_directories(config_test_rtreplac PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtreplac PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtrest2 rtrest2.c)
target_include_directories(config_test_rtrest2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtrest2 PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtrestor rtrestor.c)
target_include_directories(config_test_rtrestor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtrestor PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtsave rtsave.c)
target_include_directories(config_test_rtsave PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtsave PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtsetopt rtsetopt.c)
target_include_directories(config_test_rtsetopt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtsetopt PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtsetsec rtsetsec.c)
target_include_directories(config_test_rtsetsec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtsetsec PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtsymlnk rtsymlnk.c)
target_include_directories(config_test_rtsymlnk PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtsymlnk PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rttrecpy rttrecpy.c)
target_include_directories(config_test_rttrecpy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rttrecpy PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtunload rtunload.c)
target_include_directories(config_test_rtunload PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtunload PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_rtvbatcr rtvbatcr.c)
target_include_directories(config_test_rtvbatcr PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_rtvbatcr PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

add_executable(config_test_symtest symtest.c)
target_include_directories(config_test_symtest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(config_test_symtest PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

message(STATUS "Configured config test targets as separate executables")
