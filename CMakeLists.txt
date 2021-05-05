cmake_minimum_required(VERSION 3.17)
project(monkey)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -ggdb)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
