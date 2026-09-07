# OpenXP DLL Subsystem CMake Configuration

cmake_minimum_required(VERSION 3.15)

# Add WOW64 subsystem (Windows 7 layout)
if(CMAKE_SIZEOF_VOID_P EQUAL 8 OR (CMAKE_SIZEOF_VOID_P EQUAL 4 AND CMAKE_SYSTEM_PROCESSOR MATCHES "ARM|aarch64"))
    option(ENABLE_WOW64 "Build WOW64 subsystem" ON)
    
    if(ENABLE_WOW64)
        add_subdirectory(wow64)
        message(STATUS "WOW64 subsystem enabled")
    endif()
endif()

# Future DLL modules will be added here
# add_subdirectory(ntdll)
# add_subdirectory(kernel32)
# add_subdirectory(user32)
# etc.
