#
# WRK Compiler Flags Module
# Mirroring ReactOS build environment compiler configuration
# Supports MSVC, MinGW, GCC, Clang, and LLVM
#

macro(set_wrk_compiler_flags)
    # Common compiler definitions for all architectures
    set(WRK_COMMON_DEFINES
        CONDITION_HANDLING=1
        NT_INST=0
        WIN32=100
        _NT1X_=100
        WINNT=1
        _WIN32_WINNT=0x0502
        WINVER=0x0502
        _WIN32_IE=0x0603
        WIN32_LEAN_AND_MEAN=1
        DBG=0
        DEVL=1
        NDEBUG
        _NTSYSTEM_
        NT_SMT
        NTOS_KERNEL_RUNTIME=1
    )
    
    # Use WRK_ARCH_NAME if already set (from CMakeLists.txt), otherwise detect
    if(NOT DEFINED WRK_ARCH_NAME)
        if(WRK_ARCH STREQUAL "amd64")
            set(WRK_ARCH_NAME amd64)
        else()
            set(WRK_ARCH_NAME x86)
        endif()
    endif()
    
    if(MSVC)
        # Base compiler flags mirroring makefile.build
        set(WRK_BASE_C_FLAGS
            -Zl          # No default library name in .obj
            -Zp8         # 8-byte packing
            -Gy          # Enable function-level linking
            -W3          # Warning level 3
            -WX          # Warnings as errors
            -GR-         # Disable RTTI
            -GF          # Read-only string pooling
            -GS-         # Disable buffer security checks
            -GL-         # Disable whole program optimization
            -MT          # Static runtime (will be overridden)
            -U_MT        # Undefine _MT
            -Z7          # Debug info in .obj
            /O2          # Optimize for speed
            /Oy-         # Don't omit frame pointers
        )
        
        # Architecture-specific flags
        if(WRK_ARCH_NAME STREQUAL "amd64")
            # AMD64
            list(APPEND WRK_BASE_C_FLAGS -Wp64)
            set(WRK_MACHINE_TYPE AMD64)
        else()
            # x86
            list(APPEND WRK_BASE_C_FLAGS
                -Gm-       # Disable minimal rebuild
                -Gz        # Stdcall calling convention
                -GX-       # Disable exception handling
                -G6        # Optimize for Pentium Pro
                -Ze        # Enable language extensions
                -Gi-       # Disable intrinsic functions
                -QIfdiv-   # No FDIV bug workaround
            )
            set(WRK_MACHINE_TYPE x86)
        endif()
        
        # Linker flags mirroring WRK BUILD/makefile
        set(WRK_LINKER_FLAGS
            /IGNORE:4087,4001,4010,4037,4039,4065,4070,4078,4087,4089,4221,4198
            /WX              # Warnings as errors
            /NODEFAULTLIB    # No default libraries
            /machine:${WRK_MACHINE_TYPE}
            /driver          # Driver mode
            /OPT:REF         # Eliminate unreferenced data
            /OPT:ICF         # Identical COMDAT folding
            /INCREMENTAL:NO  # No incremental linking
            /debug           # Generate debug info
            /pdbcompress     # Compress PDB
        )
        
        # Architecture-specific linker flags
        if(WRK_ARCH_NAME STREQUAL "x86")
            list(APPEND WRK_LINKER_FLAGS
                /safeseh
                /STACK:0x40000,0x2000
                /align:0x1000
            )
            set(WRK_ENTRY_POINT "KiSystemStartup@4")
        else()
            list(APPEND WRK_LINKER_FLAGS
                /IGNORE:4108,4088,4218,4235
                /STACK:0x80000,0x2000
            )
            set(WRK_ENTRY_POINT "KiSystemStartup")
        endif()
        
        # Section merge options
        set(WRK_SECTION_MERGE
            /merge:PAGECONST=PAGE
            /merge:INITCONST=INIT
            /merge:INITDATA=INIT
            /merge:PAGELKCONST=PAGELK
            /merge:PAGEVRFY_CONST=PAGEVRFY
            /MERGE:_PAGE=PAGE
            /MERGE:_TEXT=.text
            /merge:.rdata=.text
        )
        
        # Version information
        set(WRK_VERSION_INFO
            /release
            /version:5.2
            /osversion:5.2
            /subsystem:native,5.02
        )
        
        # Convert lists to strings
        string(REPLACE ";" " " WRK_C_FLAGS_STR "${WRK_BASE_C_FLAGS}")
        string(REPLACE ";" " " WRK_LINKER_FLAGS_STR "${WRK_LINKER_FLAGS}")
        string(REPLACE ";" " " WRK_SECTION_MERGE_STR "${WRK_SECTION_MERGE}")
        string(REPLACE ";" " " WRK_VERSION_INFO_STR "${WRK_VERSION_INFO}")
        
        # Set global compiler flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WRK_C_FLAGS_STR}")
        
        # Store linker flags for later use
        set(WRK_EXE_LINKER_FLAGS "${WRK_LINKER_FLAGS_STR} ${WRK_SECTION_MERGE_STR} ${WRK_VERSION_INFO_STR}" CACHE STRING "WRK Linker Flags")
        
        # Add common definitions
        add_compile_definitions(${WRK_COMMON_DEFINES})
        
        message(STATUS "WRK Compiler: MSVC ${MSVC_VERSION}")
        message(STATUS "WRK Architecture: ${WRK_ARCH_NAME}")
        
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        # GCC/MinGW/Clang flags for cross-compilation
        set(WRK_GCC_C_FLAGS
            -fno-builtin
            -fno-pie
            -ffreestanding
            -fno-stack-protector
            -Wall
            -Werror
            -mno-red-zone
            -Wno-unused-function
            -Wno-unused-variable
        )
        
        # Windows-specific flags
        if(WIN32 OR MINGW)
            list(APPEND WRK_GCC_C_FLAGS
                -D_WIN32_WINNT=0x0502
                -DWINVER=0x0502
                -D_WIN32_IE=0x0603
                -DWIN32_LEAN_AND_MEAN
                -DNTOS_KERNEL_RUNTIME
            )
        endif()
        
        # Architecture-specific flags
        if(WRK_ARCH_NAME STREQUAL "amd64")
            list(APPEND WRK_GCC_C_FLAGS -m64)
        else()
            list(APPEND WRK_GCC_C_FLAGS 
                -m32
                -march=i586
            )
        endif()
        
        # Linker flags for GCC/MinGW/Clang
        set(WRK_GCC_LD_FLAGS
            -nostdlib
            -nodefaultlibs
            -Wl,--entry,KiSystemStartup
            -Wl,--subsystem,native
        )
        
        if(WRK_ARCH_NAME STREQUAL "x86")
            list(APPEND WRK_GCC_LD_FLAGS
                -Wl,--stack,0x40000,0x2000
            )
        else()
            list(APPEND WRK_GCC_LD_FLAGS
                -Wl,--stack,0x80000,0x2000
            )
        endif()
        
        string(REPLACE ";" " " WRK_GCC_C_FLAGS_STR "${WRK_GCC_C_FLAGS}")
        string(REPLACE ";" " " WRK_GCC_LD_FLAGS_STR "${WRK_GCC_LD_FLAGS}")
        
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WRK_GCC_C_FLAGS_STR}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WRK_GCC_LD_FLAGS_STR}")
        
        add_compile_definitions(${WRK_COMMON_DEFINES})
        
        if(MINGW)
            message(STATUS "WRK Compiler: MinGW")
        elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_LINKER MATCHES "lld")
            message(STATUS "WRK Compiler: LLVM/Clang with LLD")
        elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
            message(STATUS "WRK Compiler: Clang")
        else()
            message(STATUS "WRK Compiler: GCC")
        endif()
        message(STATUS "WRK Architecture: ${WRK_ARCH_NAME}")
    endif()
endmacro()

macro(set_wrk_include_directories)
    # Include directories mirroring WRK structure
    set(WRK_INCLUDE_PATHS
        ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
        ${CMAKE_SOURCE_DIR}/sdk/ddk/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/ds/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/sdktools/inc
        ${CMAKE_SOURCE_DIR}/sdk/internal/base/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc
        ${CMAKE_SOURCE_DIR}/sdk/sdk/inc/crt
        ${CMAKE_SOURCE_DIR}/sdk/halkit/inc
        ${CMAKE_SOURCE_DIR}/base/inc
    )
    
    # Add architecture-specific include path
    if(WRK_ARCH_NAME STREQUAL "amd64")
        list(APPEND WRK_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64)
    else()
        list(APPEND WRK_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/ntoskrnl/i386)
    endif()
    
    include_directories(${WRK_INCLUDE_PATHS})
endmacro()
