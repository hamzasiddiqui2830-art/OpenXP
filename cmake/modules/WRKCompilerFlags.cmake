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
            /merge:PAGECONST=PAGE
            /merge:INITCONST=INIT
            /merge:INITDATA=INIT
            /merge:PAGELKCONST=PAGELK
            /merge:PAGEVRFY_CONST=PAGEVRFY
            /MERGE:_PAGE=PAGE
            /MERGE:_TEXT=.text
            /merge:.rdata=.text
        )
        
        set(WRK_VERSION_INFO
            /release
            /version:5.2
            /osversion:5.2
            /subsystem:native,5.02
        )
        
        string(REPLACE ";" " " WRK_C_FLAGS_STR "${WRK_BASE_C_FLAGS}")
        string(REPLACE ";" " " WRK_LINKER_FLAGS_STR "${WRK_LINKER_FLAGS}")
        string(REPLACE ";" " " WRK_SECTION_MERGE_STR "${WRK_SECTION_MERGE}")
        string(REPLACE ";" " " WRK_VERSION_INFO_STR "${WRK_VERSION_INFO}")
        
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WRK_C_FLAGS_STR}")
        set(WRK_EXE_LINKER_FLAGS "${WRK_LINKER_FLAGS_STR} ${WRK_SECTION_MERGE_STR} ${WRK_VERSION_INFO_STR}" CACHE STRING "WRK Linker Flags")
        add_compile_definitions(${WRK_COMMON_DEFINES})
        
        message(STATUS "WRK Compiler: MSVC ${MSVC_VERSION}")
        message(STATUS "WRK Architecture: ${WRK_ARCH_NAME}")
        
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
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
        
        if(WIN32 OR MINGW)
            list(APPEND WRK_GCC_C_FLAGS
                -D_WIN32_WINNT=0x0502
                -DWINVER=0x0502
                -D_WIN32_IE=0x0603
                -DWIN32_LEAN_AND_MEAN
                -DNTOS_KERNEL_RUNTIME
            )
        endif()
        
        if(WRK_ARCH_NAME STREQUAL "amd64")
            list(APPEND WRK_GCC_C_FLAGS -m64)
        else()
            list(APPEND WRK_GCC_C_FLAGS
                -m32
                -march=i586
            )
        endif()
        
        set(WRK_GCC_LD_FLAGS
            -nostdlib
            -nodefaultlibs
            -Wl,--entry,KiSystemStartup
            -Wl,--subsystem,native
        )
        
        if(WRK_ARCH_NAME STREQUAL "x86")
            list(APPEND WRK_GCC_LD_FLAGS -Wl,--stack,0x40000,0x2000)
        else()
            list(APPEND WRK_GCC_LD_FLAGS -Wl,--stack,0x80000,0x2000)
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
    
    if(WRK_ARCH_NAME STREQUAL "amd64")
        list(APPEND WRK_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64)
    else()
        list(APPEND WRK_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/ntoskrnl/i386)
    endif()
    
    include_directories(BEFORE ${WRK_INCLUDE_PATHS})
    list(APPEND CMAKE_INCLUDE_PATH ${WRK_INCLUDE_PATHS})
    set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH}" CACHE STRING "WRK Include Paths" FORCE)
endmacro()