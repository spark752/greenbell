add_executable(basic
    basic.cpp
    )
target_link_libraries(basic greenbell)
target_compile_options(basic PRIVATE ${PROJECT_WARNINGS})
target_include_directories(basic PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
