# PO Tests CMakeLists.txt
# Each test is built as a separate executable target

# bfile test
add_executable(po_test_bfile
    bfile/bfile.c
)
target_include_directories(po_test_bfile PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bfile
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_bfile PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# ehib test
add_executable(po_test_ehib
    ehib/ehib.c
)
target_include_directories(po_test_ehib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ehib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_ehib PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# qtime test
add_executable(po_test_qtime
    qtime/qtime.c
)
target_include_directories(po_test_qtime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_qtime PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# shd test
add_executable(po_test_shd
    shd/shd.c
)
target_include_directories(po_test_shd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/shd
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_shd PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# thrd test
add_executable(po_test_thrd
    thrd/thrd.c
)
target_include_directories(po_test_thrd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thrd
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_thrd PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# usermsg test (Windows GUI app)
add_executable(po_test_usermsg WIN32
    usermsg/usermsg.c
    usermsg/usermsg.rc
)
target_include_directories(po_test_usermsg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/usermsg
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_usermsg PRIVATE _NTOS_ _NTIFS_ _NTDDK_ UNICODE _UNICODE)

# vhib test
add_executable(po_test_vhib
    vhib/vhib.c
)
target_include_directories(po_test_vhib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vhib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_vhib PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

# wtime test
add_executable(po_test_wtime
    wtime/wtime.c
)
target_include_directories(po_test_wtime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/wtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)
target_compile_definitions(po_test_wtime PRIVATE _NTOS_ _NTIFS_ _NTDDK_)

message(STATUS "Configured PO test targets as separate executables")
