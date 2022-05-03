add_executable(minimal
    minimal.cpp
    )
target_link_libraries(minimal greenbell)
target_compile_options(minimal PRIVATE ${PROJECT_WARNINGS} ${PROJECT_OPTIMIZE})
target_include_directories(minimal PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
