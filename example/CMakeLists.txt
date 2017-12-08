cmake_minimum_required(VERSION 3.1)
project(skal-example)

add_executable(example example.cpp)

set_property(TARGET example PROPERTY CXX_STANDARD 14)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET example PROPERTY CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
target_compile_options(example PUBLIC -Wall -Wextra -Werror
    -Wno-unused-parameter -Wno-nonnull-compare)
endif()

target_link_libraries(example skal)
