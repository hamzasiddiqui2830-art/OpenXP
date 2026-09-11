# nls Module CMakeLists.txt
# The WRK nls target is a NOTARGET message-source target. Keep the
# source manifest explicit and do not treat bugcodes.mc as a C source.

set(MODULE_NAME nls)
set(TARGET_NAME ntoshdrs)

set(MODULE_SOURCES
    bugcodes.mc
)

add_custom_target(${TARGET_NAME} SOURCES ${MODULE_SOURCES})

set_source_files_properties(${MODULE_SOURCES}
    PROPERTIES HEADER_FILE_ONLY TRUE
)

message(STATUS "Configured nls module: ${TARGET_NAME} with 1 source")
