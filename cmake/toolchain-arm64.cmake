# ReactOS-WRK ARM64 Toolchain File
# Configures the build for ARM64 (AArch64) architecture

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Architecture definition
set(ARCH arm64)
set(_ARM64_ 1)
set(_M_ARM64 4)

# Compiler flags for ARM64
set(CMAKE_C_FLAGS_INIT "-march=armv8-a")
set(CMAKE_CXX_FLAGS_INIT "-march=armv8-a")

# Preprocessor definitions
add_definitions(-D_ARM64_)
add_definitions(-D_M_ARM64=4)
add_definitions(-DTARGET_ARM64)

# Include paths for ARM64-specific headers
list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/ntoskrnl/inc")
list(APPEND CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/sdk/sdk/inc")

# Stack alignment for ARM64 (16-byte alignment required)
set(STACK_ALIGNMENT 16)

# Endianness (ARM64 is little-endian for Windows)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mlittle-endian")
