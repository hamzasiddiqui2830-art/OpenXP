#
# WRK Module Builder
# Mirroring ReactOS module build system
#

include(CMakeParseArguments)

function(add_wrk_module MODULE_NAME)
    set(options OPTIONAL)
    set(oneValueArgs OUTPUT_DIR TARGET_NAME)
    set(multiValueArgs SOURCES HEADERS DEPENDS)

    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT ARG_TARGET_NAME)
        set(ARG_TARGET_NAME ntos_${MODULE_NAME})
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR
            "Module '${MODULE_NAME}' must provide an explicit SOURCES list"
        )
    endif()

    if(NOT ARG_OUTPUT_DIR)
        if(MSVC)
            set(COMPILER_PREFIX "VS")
        elseif(CMAKE_C_COMPILER_ID MATCHES "GNU" AND
               CMAKE_SYSTEM_NAME MATCHES "Windows")
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

        set(ARG_OUTPUT_DIR
            ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}
        )
    endif()

    add_library(${ARG_TARGET_NAME} STATIC ${ARG_SOURCES})

    set(MODULE_INCLUDE_DIRS
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
        list(APPEND MODULE_INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64
        )
    else()
        list(APPEND MODULE_INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/ntoskrnl/i386
        )
    endif()

    target_include_directories(
        ${ARG_TARGET_NAME}
        PRIVATE
        ${MODULE_INCLUDE_DIRS}
    )

    if(MSVC AND WRK_ARCH_NAME STREQUAL "x86")
        target_compile_options(
            ${ARG_TARGET_NAME}
            PRIVATE
            /Gz
        )

        if(MODULE_NAME STREQUAL "rtl" OR MODULE_NAME STREQUAL "se")
            target_compile_options(
                ${ARG_TARGET_NAME}
                PRIVATE
                /wd4101
            )
        endif()
    endif()

    set_target_properties(
        ${ARG_TARGET_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${ARG_OUTPUT_DIR}
        POSITION_INDEPENDENT_CODE OFF
        OUTPUT_NAME ${ARG_TARGET_NAME}
    )

    if(ARG_DEPENDS)
        add_dependencies(
            ${ARG_TARGET_NAME}
            ${ARG_DEPENDS}
        )
    endif()

    message(
        STATUS
        "Added module: ${MODULE_NAME} as ${ARG_TARGET_NAME}"
    )
endfunction()

function(parse_wrk_makefile MAKEFILE_PATH OUT_VAR)
    if(NOT EXISTS ${MAKEFILE_PATH})
        return()
    endif()

    file(READ ${MAKEFILE_PATH} MAKEFILE_CONTENT)

    string(
        REGEX MATCHALL
        "asobjs[ \t]*=[ \t]*([^\n]*)"
        AS_OBJS_MATCH
        "${MAKEFILE_CONTENT}"
    )

    string(
        REGEX MATCHALL
        "ccobjs[ \t]*=[ \t]*([^\n]*)"
        CC_OBJS_MATCH
        "${MAKEFILE_CONTENT}"
    )

    string(
        REGEX MATCHALL
        "ccarchobjs[ \t]*=[ \t]*([^\n]*)"
        CCARCH_OBJS_MATCH
        "${MAKEFILE_CONTENT}"
    )

    set(OBJECT_FILES "")

    foreach(
        MATCH
        ${AS_OBJS_MATCH}
        ${CC_OBJS_MATCH}
        ${CCARCH_OBJS_MATCH}
    )
        string(
            REGEX REPLACE
            "^[a-z]+[ \t]*=[ \t]*"
            ""
            OBJ_LIST
            "${MATCH}"
        )

        string(
            REPLACE
            "\\"
            "/"
            OBJ_LIST
            "${OBJ_LIST}"
        )

        string(
            REPLACE
            " "
            ";"
            OBJ_LIST
            "${OBJ_LIST}"
        )

        foreach(OBJ ${OBJ_LIST})
            string(STRIP "${OBJ}" OBJ)

            if(OBJ AND NOT OBJ MATCHES "^\\$")
                string(
                    REPLACE
                    ".obj"
                    ".c"
                    SRC_FILE
                    "${OBJ}"
                )

                string(
                    REPLACE
                    "$(OBJ)/"
                    ""
                    SRC_FILE
                    "${SRC_FILE}"
                )

                list(APPEND OBJECT_FILES ${SRC_FILE})
            endif()
        endforeach()
    endforeach()

    set(
        ${OUT_VAR}
        ${OBJECT_FILES}
        PARENT_SCOPE
    )
endfunction()
