cmake_minimum_required(VERSION 3.19)
project(monkey)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
