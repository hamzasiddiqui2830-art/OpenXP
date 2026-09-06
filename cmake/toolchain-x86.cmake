#
# CMake toolchain file for x86 architecture
# Mirroring ReactOS build environment
#

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

# Architecture-specific settings
set(CMAKE_SIZEOF_VOID_P 4)
set(ARCH x86)

# Compiler detection
if(MSVC)
    set(CMAKE_C_COMPILER cl.exe)
    set(CMAKE_ASM_MASM_COMPILER ml.exe)
    
    # x86-specific compiler flags
    set(CMAKE_C_FLAGS_INIT "/Gm- /Gz /GX- /G6 /Ze /Gi- /QIfdiv-")
    set(CMAKE_ASM_MASM_FLAGS_INIT "-safeseh -coff -Zm")
endif()

# Target architecture
set(CMAKE_GENERATOR_PLATFORM Win32 CACHE STRING "Platform" FORCE)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
