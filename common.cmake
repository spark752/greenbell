if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS
        "CMAKE_BUILD_TYPE defaulting to 'Debug'")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(COMPILE_OPTIONS_C -O2 -Wall)

set(PROJECT_WARNINGS
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wcast-align
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
)
set(PROJECT_OPTIMIZE
    -O2 
    -msse3
)

option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} "--header-filter='.*'")
        message(STATUS "ENABLE_CLANG_TIDY = On")
    else()
        message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
endif()

option(ENABLE_INCLUDE_WHAT_YOU_USE
    "Enable static analysis with include-what-you-use" OFF)
if(ENABLE_INCLUDE_WHAT_YOU_USE)
    find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
    if(INCLUDE_WHAT_YOU_USE)
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
        message(STATUS, "ENABLE_INCLUDE_WHAT_YOU_USE = On")
    else()
        message(SEND_ERROR
            "include-what-you-use requested but executable not found")
    endif()
endif()

option(USE_CLANG_LIBC "Use Clang's libc++ as the C++ library" OFF)
if(USE_CLANG_LIBC)
    string(APPEND CMAKE_CXX_FLAGS "-stdlib=libc++")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "-lc++abi")
    message(STATUS
        "USE_CLANG_LIBC = Will use Clang's libc++ as the C++ library")
else()
    message(STATUS "Will use libstdc++ as the C++ library")
endif()
