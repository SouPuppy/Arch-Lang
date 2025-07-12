cmake_minimum_required(VERSION 3.25)

project(arch-lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/libcore)
add_subdirectory(src/libsyntax)
add_subdirectory(src/arch)
