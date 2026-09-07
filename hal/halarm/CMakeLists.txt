cmake_minimum_required(VERSION 3.15)
project(halarm C ASM)

# HAL ARM Sources
set(HALARM_SOURCES
    halarm.c
    interrupt.c
    timer.c
    bus.c
    dma.c
)

# Add architecture-specific sources based on compiler
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang assembly files
    # TODO: Add ARM assembly files when available
    # set(HALARM_ASM_SOURCES arch/arm/start.S)
elseif(MSVC)
    # MSVC doesn't support ARM assembly directly in CMake
endif()

# Create the HAL ARM library
add_library(halarm STATIC ${HALARM_SOURCES})

# Set include directories
target_include_directories(halarm
    PRIVATE
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/hal/halarm
)

# Add compiler definitions
target_compile_definitions(halarm
    PRIVATE
        _ARM_
        _HAL_ARM_
        $<$<CONFIG:Debug>:DBG=1>
)

# Set ARM-specific compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(halarm
        PRIVATE
            -march=armv7-a
            -mthumb
            -mfloat-abi=softfp
    )
elseif(MSVC)
    target_compile_options(halarm
        PRIVATE
            /arm
    )
endif()

# Export symbols
set_target_properties(halarm
    PROPERTIES
        OUTPUT_NAME "halarm"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
