#
# CMake toolchain file for amd64 architecture
# Mirroring ReactOS build environment
#

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Architecture-specific settings
set(CMAKE_SIZEOF_VOID_P 8)
set(ARCH amd64)

# Compiler detection
if(MSVC)
    set(CMAKE_C_COMPILER cl.exe)
    set(CMAKE_ASM_MASM_COMPILER ml64.exe)
    
    # amd64-specific compiler flags
    set(CMAKE_C_FLAGS_INIT "/Wp64")
    set(CMAKE_ASM_MASM_FLAGS_INIT "")
endif()

# Target architecture
set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "Platform" FORCE)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
