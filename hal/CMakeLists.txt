# Add HAL stub source file - provides HAL implementations without DLL dependency
add_library(hal_stub STATIC ${CMAKE_CURRENT_SOURCE_DIR}/hal_stub.c)

target_include_directories(hal_stub PRIVATE
    ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
    ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
)

set_target_properties(hal_stub PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE OFF
)

# HAL target libraries for different HAL configurations

# halarm - HAL ARM (replaces legacy halia64)
if(ENABLE_HAL_ARM)
    add_subdirectory(halarm)
endif()

# halacpim - HAL ACPI Multiprocessor
add_library(halacpim STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/halacpim/halacpim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_stub.c
)
target_include_directories(halacpim PRIVATE
    ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
    ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
)
set_target_properties(halacpim PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE OFF
)

# halmacpi - HAL ACPI
add_library(halmacpi STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/halmacpi/halmacpi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_stub.c
)
target_include_directories(halmacpi PRIVATE
    ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
    ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
)
set_target_properties(halmacpi PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE OFF
)

# halmps - HAL Multiprocessor System
add_library(halmps STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/halmps/halmps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_stub.c
)
target_include_directories(halmps PRIVATE
    ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
    ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
    ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
    ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
)
set_target_properties(halmps PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    POSITION_INDEPENDENT_CODE OFF
)
