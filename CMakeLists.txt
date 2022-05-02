cmake_minimum_required(VERSION 3.7.2)
project(Greenbell)

include(common.cmake)

option(DEBUG_TIMING "Show timing info from threads. DEBUG ONLY" OFF)
option(DEBUG_AUDIO "Show debug info for audio. DEBUG ONLY" OFF)
option(USE_GET_ERROR "Include glGetError calls for debugging" ON)
option(BUILD_DEMO_APPS "Build optional demo applications" ON)
option(BUILD_TEST_APPS "Build test applications" ON)

find_library(SDL_LIB SDL2)
message(STATUS "SDL2: ${SDL_LIB}")

find_library(SDL_TTF SDL2_ttf)
message(STATUS "SDL2_ttf: ${SDL_TTF}")
find_library(PTHREAD_LIB pthread) # std::thread uses pthread (at least on Linux)
find_library(DL_LIB libdl.so dl) # Required by Glad

# Create a header file with options provided via cmake
configure_file(cmake_config.h.in cmake_config.h @ONLY)

# Make Glad object library
message(STATUS "USE_OPENGL_4 = Configuring for OpenGL 4")
set(GLAD_SRC engine/glad/glad.c)
set(GLAD_INCLUDE include/glad)

# Make main library which includes the others
add_library(greenbell STATIC
    engine/window.cpp
    engine/log.cpp
    ${GLAD_SRC}
)

# PROJECT_WARNINGS and PROJECT_OPTIMIZE come from the common.cmake file
target_compile_options(greenbell PRIVATE ${PROJECT_WARNINGS} ${PROJECT_OPTIMIZE})
target_link_libraries(greenbell
    INTERFACE ${DL_LIB} ${SDL_LIB} ${SDL_TTF} ${PTHREAD_LIB}
)
target_include_directories(greenbell
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_INCLUDE}>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:${GLAD_INCLUDE}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/${GLAD_INCLUDE}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Target is a static library named based on the OpenGL version supported
# This lets you link your applications against whichever you want
set_target_properties(greenbell PROPERTIES PREFIX "")
set_target_properties(greenbell PROPERTIES OUTPUT_NAME "greenbell_gl4")
set_target_properties(greenbell PROPERTIES SUFFIX ".a")

# Optional apps that have their own cmake files
if (BUILD_DEMO_APPS)
    message(STATUS "BUILD_DEMO_APPS = Demo applications will be built")
    add_subdirectory(demos)
endif()
if (BUILD_TEST_APPS)
    message(STATUS "BUILD_TEST_APPS = Test applications will be built")
    add_subdirectory(tests)
endif()

# Package
# This allows projects to find_package Greenbell and know its dependencies
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/greenbell-config.cmake"
    INSTALL_DESTINATION "."
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/greenbell-config-version.cmake"
    VERSION "0.1"
    COMPATIBILITY AnyNewerVersion
)
