# ReactOS-WRK ARM Toolchain File
# Configures the build for ARM (32-bit) architecture

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Architecture definition
set(ARCH arm)
set(_ARM_ 1)
set(_M_ARM 4)

# Compiler flags for ARM
set(CMAKE_C_FLAGS_INIT "-march=armv7-a -mtune=cortex-a9")
set(CMAKE_CXX_FLAGS_INIT "-march=armv7-a -mtune=cortex-a9")

# Preprocessor definitions
add_definitions(-D_ARM_)
add_definitions(-D_M_ARM=4)
add_definitions(-DTARGET_ARM)

# Include paths for ARM-specific headers
list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ntoskrnl/inc")
list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/sdk/sdk/inc")

# Stack alignment for ARM
set(STACK_ALIGNMENT 8)

# Endianness (ARM can be both, but typically little-endian for Windows)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mlittle-endian")
