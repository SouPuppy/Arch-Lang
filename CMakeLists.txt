cmake_minimum_required(VERSION 3.25)

project(arch-lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/core)
add_subdirectory(src/syntax)
add_subdirectory(./src/arch)
add_subdirectory(./src/libicli)

add_subdirectory(./example)
