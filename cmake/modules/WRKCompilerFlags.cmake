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
    
    if(NOT DEFINED WRK_ARCH_NAME)
        if(WRK_ARCH STREQUAL "amd64")
            set(WRK_ARCH_NAME amd64)
        else()
            set(WRK_ARCH_NAME x86)
        endif()
    endif()
    
    if(MSVC)
        set(WRK_BASE_C_FLAGS
            -nostdinc
            -Zl
            -Zp8
            -Gy
            -W3
            -WX
            -GR-
            -GF
            -GS-
            -GL-
            -MT
            -U_MT
            -Z7
            /O2
            /Oy-
            /wd4101
            /wd4005
            /FI${CMAKE_SOURCE_DIR}/ntoskrnl/inc/wrk_msvc_compat.h
        )
        
        if(WRK_ARCH_NAME STREQUAL "amd64")
            list(APPEND WRK_BASE_C_FLAGS -Wp64)
            set(WRK_MACHINE_TYPE AMD64)
        else()
            list(APPEND WRK_BASE_C_FLAGS
                -Gm-
                -Gz
                -GX-
                -G6
                -Ze
                -Gi-
                -QIfdiv-
            )
            set(WRK_MACHINE_TYPE x86)
        endif()
        
        set(WRK_LINKER_FLAGS
            /IGNORE:4087,4001,4010,4037,4039,4065,4070,4078,4087,4089,4221,4198
            /WX
            /NODEFAULTLIB
            /machine:${WRK_MACHINE_TYPE}
            /driver
            /OPT:REF
            /OPT:ICF
            /INCREMENTAL:NO
            /debug
            /pdbcompress
        )
        
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
        
        set(WRK_SECTION_MERGE
