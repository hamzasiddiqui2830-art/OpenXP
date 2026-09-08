# Add HAL stub source file - provides HAL implementations without DLL dependency
add_library(hal_stub STATIC ${REACTOS_SOURCE_DIR}/hal/hal_stub.c)

target_include_directories(hal_stub PRIVATE
    ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
    ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
    ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
    ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
    ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
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
        ${REACTOS_SOURCE_DIR}/hal/up/i386/bushnd.c
        ${REACTOS_SOURCE_DIR}/hal/up/i386/chiphacks.c
        ${REACTOS_SOURCE_DIR}/hal/up/i386/drivesup.c
        ${REACTOS_SOURCE_DIR}/hal/up/i386/rangesup.c
    )
    target_include_directories(halup PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
        ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
        ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
        ${REACTOS_SOURCE_DIR}/hal/up/i386
    )
    set_target_properties(halup PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halacpi - HAL ACPI PIC
if(ENABLE_HAL_ACPI)
    file(GLOB HALACPI_SOURCES
        ${REACTOS_SOURCE_DIR}/hal/acpi/i386*.c
    )
    add_library(halacpi STATIC ${HALACPI_SOURCES})
    target_include_directories(halacpi PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
        ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
        ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
        ${REACTOS_SOURCE_DIR}/hal/acpi/i386
        ${REACTOS_SOURCE_DIR}/hal/acpi/i386/apic
        ${REACTOS_SOURCE_DIR}/hal/acpi/i386/mpapic
        ${REACTOS_SOURCE_DIR}/hal/acpi/i386/picacpi
    )
    set_target_properties(halacpi PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halmps - HAL Multiprocessor System
if(ENABLE_HAL_MPS)
    file(GLOB HALMPS_SOURCES
        ${REACTOS_SOURCE_DIR}/hal/mps/i386*.c
        ${REACTOS_SOURCE_DIR}/hal/mps/i386/mp*.c
        ${REACTOS_SOURCE_DIR}/hal/mps/i386/up*.c
    )
    add_library(halmps STATIC ${HALMPS_SOURCES})
    target_include_directories(halmps PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
        ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
        ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
        ${REACTOS_SOURCE_DIR}/hal/mps/i386
        ${REACTOS_SOURCE_DIR}/hal/mps/i386/mp
        ${REACTOS_SOURCE_DIR}/hal/mps/i386/up
    )
    set_target_properties(halmps PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        POSITION_INDEPENDENT_CODE OFF
    )
endif()

# halapic - HAL ACPI APIC
if(ENABLE_HAL_APIC)
    file(GLOB HALAPIC_SOURCES
        ${REACTOS_SOURCE_DIR}/hal/apic/i386*.c
    )
    add_library(halapic STATIC ${HALAPIC_SOURCES})
    target_include_directories(halapic PRIVATE
        ${REACTOS_SOURCE_DIR}/ntoskrnl/inc
        ${REACTOS_SOURCE_DIR}/sdk/ddk/inc
        ${REACTOS_SOURCE_DIR}/sdk/internal/ds/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc
        ${REACTOS_SOURCE_DIR}/sdk/sdk/inc/crt
        ${REACTOS_SOURCE_DIR}/hal/apic/i386
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
