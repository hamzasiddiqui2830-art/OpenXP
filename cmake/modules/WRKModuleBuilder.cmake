#
# WRK Module Builder
# Explicit WRK-style module/target builder.
#
# Design:
#   - No filesystem source discovery.
#   - No GLOB/GLOB_RECURSE.
#   - No SOURCE_DIR-based source inference.
#   - No automatic architecture detection.
#   - Each target must provide an explicit SOURCES list.
#   - Architecture-specific source selection belongs in the
#     module's CMakeLists.txt and uses the explicit ARCH variable.
#

include(CMakeParseArguments)


#
# add_wrk_module
#
# Create one WRK module target from an explicit source manifest.
#
# Example:
#
#   set(MODULE_SOURCES
#       foo.c
#       bar.c
#   )
#
#   if(ARCH STREQUAL "i386")
#       list(APPEND MODULE_SOURCES
#           i386/foo.asm
#           i386/bar.asm
#       )
#   elseif(ARCH STREQUAL "amd64")
#       list(APPEND MODULE_SOURCES
#           amd64/foo.asm
#           amd64/bar.asm
#       )
#   endif()
#
#   add_wrk_module(
#       ke
#       TARGET_NAME ntos_ke
#       SOURCES ${MODULE_SOURCES}
#   )
#
function(add_wrk_module MODULE_NAME)

    set(options OPTIONAL)

    set(oneValueArgs
        OUTPUT_DIR
        TARGET_NAME
    )

    set(multiValueArgs
        SOURCES
        HEADERS
        DEPENDS
    )

    cmake_parse_arguments(
        ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )


    #
    # Target name
    #
    if(NOT ARG_TARGET_NAME)
        set(ARG_TARGET_NAME ntos_${MODULE_NAME})
    endif()


    #
    # SOURCES are mandatory.
    #
    # The module builder deliberately does not discover sources from
    # the filesystem. The module CMakeLists.txt must provide them.
    #
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR
            "Module '${MODULE_NAME}' must provide an explicit SOURCES list"
        )
    endif()


    #
    # Output directory
    #
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

        set(
            ARG_OUTPUT_DIR
            ${CMAKE_BINARY_DIR}/obj/${COMPILER_PREFIX}-${WRK_ARCH_NAME}
        )

    endif()


    #
    # Create the target from the explicit source list.
    #
    add_library(
        ${ARG_TARGET_NAME}
        STATIC
        ${ARG_SOURCES}
    )


    #
    # Common WRK include directories.
    #
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


    #
    # Architecture include directory.
    #
    # ARCH is an explicit build input. This block does not detect the
    # architecture; it only selects the already-requested architecture.
    #
    if(ARCH STREQUAL "i386")

        list(APPEND MODULE_INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/ntoskrnl/i386
        )

    elseif(ARCH STREQUAL "amd64")

        list(APPEND MODULE_INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/ntoskrnl/amd64
        )

    else()

        message(FATAL_ERROR
            "Unsupported WRK architecture '${ARCH}' "
            "for module '${MODULE_NAME}'"
        )

    endif()


    #
    # Apply include directories.
    #
    target_include_directories(
        ${ARG_TARGET_NAME}
        PRIVATE
        ${MODULE_INCLUDE_DIRS}
    )


    #
    # x86 WRK calling convention.
    #
    if(MSVC AND ARCH STREQUAL "i386")

        target_compile_options(
            ${ARG_TARGET_NAME}
            PRIVATE
            /Gz
        )


        #
        # WRK/NT source contains intentionally unused locals in these
        # modules. Preserve the existing warning handling.
        #
        if(MODULE_NAME STREQUAL "rtl" OR
           MODULE_NAME STREQUAL "se")

            target_compile_options(
                ${ARG_TARGET_NAME}
                PRIVATE
                /wd4101
            )

        endif()

    endif()


    #
    # Target properties.
    #
    set_target_properties(
        ${ARG_TARGET_NAME}
        PROPERTIES

        ARCHIVE_OUTPUT_DIRECTORY
            ${ARG_OUTPUT_DIR}

        POSITION_INDEPENDENT_CODE
            OFF

        OUTPUT_NAME
            ${ARG_TARGET_NAME}
    )


    #
    # Explicit target dependencies.
    #
    if(ARG_DEPENDS)

        add_dependencies(
            ${ARG_TARGET_NAME}
            ${ARG_DEPENDS}
        )

    endif()


    #
    # Informational message.
    #
    message(
        STATUS
        "Added WRK module: ${MODULE_NAME} as ${ARG_TARGET_NAME}"
    )

endfunction()


#
# parse_wrk_makefile
#
# Parse explicit WRK makefile object lists.
#
# This helper does not scan the source tree. It only parses the
# specified WRK makefile.
#
function(parse_wrk_makefile MAKEFILE_PATH OUT_VAR)

    if(NOT EXISTS ${MAKEFILE_PATH})
        return()
    endif()


    file(
        READ
        ${MAKEFILE_PATH}
        MAKEFILE_CONTENT
    )


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

            string(
                STRIP
                "${OBJ}"
                OBJ
            )


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


                list(
                    APPEND
                    OBJECT_FILES
                    ${SRC_FILE}
                )

            endif()

        endforeach()

    endforeach()


    set(
        ${OUT_VAR}
        ${OBJECT_FILES}
        PARENT_SCOPE
    )

endfunction()
