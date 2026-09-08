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
# Note: Real HAL implementations require complete source trees with assembly files
# Currently using available stub sources from hals directory

# halup - Uniprocessor HAL
if(ENABLE_HAL_UP)
    add_library(halup STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/up/i386/bushnd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/up/i386/chiphacks.c
        ${CMAKE_CURRENT_SOURCE_DIR}/up/i386/drivesup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/up/i386/rangesup.c
    )
    target_include_directories(halup PRIVATE
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
        ${CMAKE_CURRENT_SOURCE_DIR}/up/i386
    )
    set_target_properties(halup PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halacpi - HAL ACPI PIC
if(ENABLE_HAL_ACPI)
    file(GLOB HALACPI_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/acpi/i386/*.c
    )
    add_library(halacpi STATIC ${HALACPI_SOURCES})
    target_include_directories(halacpi PRIVATE
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
        ${CMAKE_CURRENT_SOURCE_DIR}/acpi/i386
        ${CMAKE_CURRENT_SOURCE_DIR}/acpi/i386/apic
        ${CMAKE_CURRENT_SOURCE_DIR}/acpi/i386/mpapic
        ${CMAKE_CURRENT_SOURCE_DIR}/acpi/i386/picacpi
    )
    set_target_properties(halacpi PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halmps - HAL Multiprocessor System
if(ENABLE_HAL_MPS)
    file(GLOB HALMPS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386/mp/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386/up/*.c
    )
    add_library(halmps STATIC ${HALMPS_SOURCES})
    target_include_directories(halmps PRIVATE
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386/mp
        ${CMAKE_CURRENT_SOURCE_DIR}/mps/i386/up
    )
    set_target_properties(halmps PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halapic - HAL ACPI APIC
if(ENABLE_HAL_APIC)
    file(GLOB HALAPIC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/apic/i386/*.c
    )
    add_library(halapic STATIC ${HALAPIC_SOURCES})
    target_include_directories(halapic PRIVATE
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
        ${CMAKE_CURRENT_SOURCE_DIR}/apic/i386
    )
    set_target_properties(halapic PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halarm - HAL ARM (replaces legacy halia64)
if(ENABLE_HAL_ARM)
    add_subdirectory(halarm)
endif()
