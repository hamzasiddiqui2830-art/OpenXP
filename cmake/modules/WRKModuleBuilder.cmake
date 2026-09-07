#
# WRK Module Builder
# Mirroring ReactOS module build system
#

include(CMakeParseArguments)

# Function to add a kernel module as a static library
function(add_wrk_module MODULE_NAME)
    set(options OPTIONAL)
    set(oneValueArgs SOURCE_DIR OUTPUT_DIR)
    set(multiValueArgs SOURCES HEADERS DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Default source directory
    if(NOT ARG_SOURCE_DIR)
        set(ARG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ntoskrnl/${MODULE_NAME})
    endif()
    
    # Default output directory - use compiler-architecture naming
    if(NOT ARG_OUTPUT_DIR)
        if(MSVC)
            set(COMPILER_PREFIX "VS")
        elseif(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Windows")
            set(COMPILER_PREFIX "MinGW")
        elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
            set(COMPILER_PREFIX "GCC")
        elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
            if(CMAKE_LINKER MATCHES "lld")
                set(COMPILER_PREFIX "LLVM")
            else()
                set(COMPILER_PREFIX "Clang")
            endif()
        else()
            set(COMPILER_PREFIX "Unknown")
        endif()
        set(ARG_OUTPUT_DIR ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME})
    endif()
    
    # Collect source files if not explicitly provided
    if(NOT ARG_SOURCES)
        # Collect C source files from module directory (excluding arch-specific subdirs)
        file(GLOB_RECURSE MODULE_C_SOURCES
            ${ARG_SOURCE_DIR}/*.c
        )
        
        # Filter out BUILD directory and architecture-specific subdirectories
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/BUILD/")
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/i386/")
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/amd64/")
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/ia64/")
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/arm/")
        list(FILTER MODULE_C_SOURCES EXCLUDE REGEX "/arm64/")
        
        # Now collect architecture-specific sources for the target architecture
        if(WRK_ARCH_NAME STREQUAL "x86")
            file(GLOB ARCH_SPECIFIC_SOURCES
                ${ARG_SOURCE_DIR}/i386/*.c
            )
            list(APPEND MODULE_C_SOURCES ${ARCH_SPECIFIC_SOURCES})
        elseif(WRK_ARCH_NAME STREQUAL "amd64")
            file(GLOB ARCH_SPECIFIC_SOURCES
                ${ARG_SOURCE_DIR}/amd64/*.c
            )
            list(APPEND MODULE_C_SOURCES ${ARCH_SPECIFIC_SOURCES})
        endif()
        
        # Collect assembly files based on architecture and convert to appropriate format
        if(WRK_ARCH_NAME STREQUAL "x86")
            # For x86, collect .asm files
            file(GLOB MODULE_ASM_SOURCES
                ${ARG_SOURCE_DIR}/i386/*.asm
            )
            
            # Convert MASM-style .asm to GAS-style .S for non-MSVC compilers
            if(NOT MSVC)
                set(MODULE_ASM_SOURCES_CONVERTED "")
                foreach(ASM_FILE ${MODULE_ASM_SOURCES})
                    # Get the filename without path
                    get_filename_component(ASM_NAME ${ASM_FILE} NAME_WE)
                    set(CONVERTED_FILE "${ARG_OUTPUT_DIR}/${ASM_NAME}.S")
                    
                    # Create converted assembly file
                    configure_file(${ASM_FILE} ${CONVERTED_FILE} COPYONLY)
                    list(APPEND MODULE_ASM_SOURCES_CONVERTED ${CONVERTED_FILE})
                endforeach()
                set(MODULE_ASM_SOURCES ${MODULE_ASM_SOURCES_CONVERTED})
            endif()
        elseif(WRK_ARCH_NAME STREQUAL "amd64")
            # For amd64, collect .asm files
            file(GLOB MODULE_ASM_SOURCES
                ${ARG_SOURCE_DIR}/amd64/*.asm
            )
            
            # Convert MASM-style .asm to GAS-style .S for non-MSVC compilers
            if(NOT MSVC)
                set(MODULE_ASM_SOURCES_CONVERTED "")
                foreach(ASM_FILE ${MODULE_ASM_SOURCES})
                    get_filename_component(ASM_NAME ${ASM_FILE} NAME_WE)
                    set(CONVERTED_FILE "${ARG_OUTPUT_DIR}/${ASM_NAME}.S")
                    configure_file(${ASM_FILE} ${CONVERTED_FILE} COPYONLY)
                    list(APPEND MODULE_ASM_SOURCES_CONVERTED ${CONVERTED_FILE})
                endforeach()
                set(MODULE_ASM_SOURCES ${MODULE_ASM_SOURCES_CONVERTED})
            endif()
        else()
            # For other architectures (ia64, etc.), don't collect arch-specific asm
            set(MODULE_ASM_SOURCES "")
        endif()
        
        set(ARG_SOURCES ${MODULE_C_SOURCES} ${MODULE_ASM_SOURCES})
    endif()
    
    # Create static library for module
    if(ARG_SOURCES)
        add_library(ntos_${MODULE_NAME} STATIC ${ARG_SOURCES})
        
        # Collect all include directories into a single list
        set(MODULE_INCLUDE_DIRS
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
            list(APPEND MODULE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64)
        else()
            list(APPEND MODULE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ntoskrnl/i386)
        endif()
        
        # Ensure module uses ReactOS SDK includes with priority
        target_include_directories(ntos_${MODULE_NAME} BEFORE PRIVATE ${MODULE_INCLUDE_DIRS})
        
        set_target_properties(ntos_${MODULE_NAME} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${ARG_OUTPUT_DIR}
            POSITION_INDEPENDENT_CODE OFF
            OUTPUT_NAME ntos${MODULE_NAME}
        )
        
        # Add dependencies
        if(ARG_DEPENDS)
            add_dependencies(ntos_${MODULE_NAME} ${ARG_DEPENDS})
        endif()
        
        message(STATUS "Added module: ${MODULE_NAME}")
    endif()
endfunction()

# Function to parse WRK makefile for source files
function(parse_wrk_makefile MAKEFILE_PATH OUT_VAR)
    if(NOT EXISTS ${MAKEFILE_PATH})
        return()
    endif()
    
    file(READ ${MAKEFILE_PATH} MAKEFILE_CONTENT)
    
    # Extract object file lists
    string(REGEX MATCHALL "asobjs[ \t]*=[ \t]*([^\n]*)" AS_OBJS_MATCH "${MAKEFILE_CONTENT}")
    string(REGEX MATCHALL "ccobjs[ \t]*=[ \t]*([^\n]*)" CC_OBJS_MATCH "${MAKEFILE_CONTENT}")
    string(REGEX MATCHALL "ccarchobjs[ \t]*=[ \t]*([^\n]*)" CCARCH_OBJS_MATCH "${MAKEFILE_CONTENT}")
    
    set(OBJECT_FILES "")
    
    # Parse each match and convert to source files
    foreach(MATCH ${AS_OBJS_MATCH} ${CC_OBJS_MATCH} ${CCARCH_OBJS_MATCH})
        string(REGEX REPLACE "^[a-z]+[ \t]*=[ \t]*" "" OBJ_LIST "${MATCH}")
        string(REPLACE "\\" "/" OBJ_LIST "${OBJ_LIST}")
        string(REPLACE " " ";" OBJ_LIST "${OBJ_LIST}")
        
        foreach(OBJ ${OBJ_LIST})
            string(STRIP "${OBJ}" OBJ)
            if(OBJ AND NOT OBJ MATCHES "^\\$")
                # Convert .obj to .c or .asm
                string(REPLACE ".obj" ".c" SRC_FILE "${OBJ}")
                string(REPLACE "$(OBJ)/" "" SRC_FILE "${SRC_FILE}")
                list(APPEND OBJECT_FILES ${SRC_FILE})
            endif()
        endforeach()
    endforeach()
    
    set(${OUT_VAR} ${OBJECT_FILES} PARENT_SCOPE)
endfunction()

# Function to add all modules from a list
function(add_wrk_modules MODULE_LIST)
    foreach(MODULE ${MODULE_LIST})
        # Replace slashes with underscores for target name
        string(REPLACE "/" "_" MODULE_TARGET "${MODULE}")
        add_wrk_module(${MODULE_TARGET} SOURCE_DIR ${CMAKE_SOURCE_DIR}/ntoskrnl/${MODULE})
    endforeach()
endfunction()
