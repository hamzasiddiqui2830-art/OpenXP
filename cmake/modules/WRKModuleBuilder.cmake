#
# WRK Module Builder
# Mirroring ReactOS module build system
#

include(CMakeParseArguments)

function(add_wrk_module MODULE_NAME)
    set(options OPTIONAL)
    set(oneValueArgs SOURCE_DIR OUTPUT_DIR TARGET_NAME)
    set(multiValueArgs SOURCES HEADERS DEPENDS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOURCE_DIR)
        set(ARG_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ntoskrnl/${MODULE_NAME})
    endif()

    if(NOT ARG_TARGET_NAME)
        set(ARG_TARGET_NAME ntos_${MODULE_NAME})
    endif()

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

    if(NOT ARG_SOURCES)
        file(GLOB_RECURSE MODULE_C_SOURCES CONFIGURE_DEPENDS "${ARG_SOURCE_DIR}/*.c")
        set(MODULE_C_SOURCES_FILTERED "")
        foreach(SOURCE_FILE IN LISTS MODULE_C_SOURCES)
            file(TO_CMAKE_PATH "${SOURCE_FILE}" SOURCE_FILE_NORMALIZED)
            if(SOURCE_FILE_NORMALIZED MATCHES "(^|/)tests?(/|$)" OR
               SOURCE_FILE_NORMALIZED MATCHES "(^|/)BUILD(/|$)" OR
               SOURCE_FILE_NORMALIZED MATCHES "(^|/)(i386|amd64|ia64|arm|arm64)(/|$)")
                continue()
            endif()
            if(MODULE_NAME STREQUAL "rtl" AND
               SOURCE_FILE_NORMALIZED MATCHES "(^|/)(generr|heapdbg|heapdll|heapleak|heaplowf)\\.c$")
                continue()
            endif()
            list(APPEND MODULE_C_SOURCES_FILTERED "${SOURCE_FILE}")
        endforeach()
        set(MODULE_C_SOURCES ${MODULE_C_SOURCES_FILTERED})

        if(WRK_ARCH_NAME STREQUAL "x86")
            file(GLOB ARCH_SPECIFIC_SOURCES CONFIGURE_DEPENDS "${ARG_SOURCE_DIR}/i386/*.c")
            list(APPEND MODULE_C_SOURCES ${ARCH_SPECIFIC_SOURCES})
        elseif(WRK_ARCH_NAME STREQUAL "amd64")
            file(GLOB ARCH_SPECIFIC_SOURCES CONFIGURE_DEPENDS "${ARG_SOURCE_DIR}/amd64/*.c")
            list(APPEND MODULE_C_SOURCES ${ARCH_SPECIFIC_SOURCES})
        endif()

        if(WRK_ARCH_NAME STREQUAL "x86")
            file(GLOB MODULE_ASM_SOURCES CONFIGURE_DEPENDS "${ARG_SOURCE_DIR}/i386/*.asm")
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
        elseif(WRK_ARCH_NAME STREQUAL "amd64")
            file(GLOB MODULE_ASM_SOURCES CONFIGURE_DEPENDS "${ARG_SOURCE_DIR}/amd64/*.asm")
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
            set(MODULE_ASM_SOURCES "")
        endif()

        set(ARG_SOURCES ${MODULE_C_SOURCES} ${MODULE_ASM_SOURCES})
    endif()

    if(ARG_SOURCES)
        add_library(${ARG_TARGET_NAME} STATIC ${ARG_SOURCES})

        # Private kernel headers such as exp.h include sibling module headers
        # such as ke/ki.h. The executive debugger sources also include kdp.h,
        # which is shared by the kd64 debugger implementation and contains the
        # x86-specific breakpoint definitions behind _X86_. Keep all private
        # kernel directories explicit instead of relying on source-directory
        # lookup behavior.
        set(MODULE_INCLUDE_DIRS
            ${ARG_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/ntoskrnl/ke
            ${CMAKE_SOURCE_DIR}/ntoskrnl/kd64
            ${CMAKE_SOURCE_DIR}/ntoskrnl/inc
            ${CMAKE_SOURCE_DIR}/ntoskrnl/rtl
            ${CMAKE_SOURCE_DIR}/ntos-old/rtl
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
            list(APPEND MODULE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64)
        else()
            list(APPEND MODULE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ntoskrnl/i386)
        endif()

        target_include_directories(${ARG_TARGET_NAME} PRIVATE ${MODULE_INCLUDE_DIRS})

        # WRK x86 kernel entry points are stdcall. The public ntexapi.h
        # declarations use NTAPI (__stdcall), while the converted C sources
        # retain the historical definition spelling without NTAPI. /Gz makes
        # those definitions use the same calling convention and prevents MSVC
        # C2373 redefinition errors.
        if(MSVC AND WRK_ARCH_NAME STREQUAL "x86")
            target_compile_options(${ARG_TARGET_NAME} PRIVATE /Gz)
            if(MODULE_NAME STREQUAL "rtl")
                target_compile_options(${ARG_TARGET_NAME} PRIVATE /wd4101)
            endif()
        endif()

        set_target_properties(${ARG_TARGET_NAME} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${ARG_OUTPUT_DIR}
            POSITION_INDEPENDENT_CODE OFF
            OUTPUT_NAME ${ARG_TARGET_NAME}
        )

        if(ARG_DEPENDS)
            add_dependencies(${ARG_TARGET_NAME} ${ARG_DEPENDS})
        endif()

        message(STATUS "Added module: ${MODULE_NAME} as ${ARG_TARGET_NAME}")
    endif()
endfunction()

function(parse_wrk_makefile MAKEFILE_PATH OUT_VAR)
    if(NOT EXISTS ${MAKEFILE_PATH})
        return()
    endif()
    file(READ ${MAKEFILE_PATH} MAKEFILE_CONTENT)
    string(REGEX MATCHALL "asobjs[ \t]*=[ \t]*([^\n]*)" AS_OBJS_MATCH "${MAKEFILE_CONTENT}")
    string(REGEX MATCHALL "ccobjs[ \t]*=[ \t]*([^\n]*)" CC_OBJS_MATCH "${MAKEFILE_CONTENT}")
    string(REGEX MATCHALL "ccarchobjs[ \t]*=[ \t]*([^\n]*)" CCARCH_OBJS_MATCH "${MAKEFILE_CONTENT}")
    set(OBJECT_FILES "")
    foreach(MATCH ${AS_OBJS_MATCH} ${CC_OBJS_MATCH} ${CCARCH_OBJS_MATCH})
        string(REGEX REPLACE "^[a-z]+[ \t]*=[ \t]*" "" OBJ_LIST "${MATCH}")
        string(REPLACE "\\" "/" OBJ_LIST "${OBJ_LIST}")
        string(REPLACE " " ";" OBJ_LIST "${OBJ_LIST}")
        foreach(OBJ ${OBJ_LIST})
            string(STRIP "${OBJ}" OBJ)
            if(OBJ AND NOT OBJ MATCHES "^\\$")
                string(REPLACE ".obj" ".c" SRC_FILE "${OBJ}")
                string(REPLACE "$(OBJ)/" "" SRC_FILE "${SRC_FILE}")
                list(APPEND OBJECT_FILES ${SRC_FILE})
            endif()
        endforeach()
    endforeach()
    set(${OUT_VAR} ${OBJECT_FILES} PARENT_SCOPE)
endfunction()

function(add_wrk_modules MODULE_LIST)
    foreach(MODULE ${MODULE_LIST})
        string(REPLACE "/" "_" MODULE_TARGET "${MODULE}")
        add_wrk_module(${MODULE_TARGET} SOURCE_DIR ${CMAKE_SOURCE_DIR}/ntoskrnl/${MODULE})
    endforeach()
endfunction()