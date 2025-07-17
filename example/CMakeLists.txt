add_executable(example_icli ./icli/main.cpp)
target_link_libraries(example_icli arch_icli)

target_include_directories(example_icli
PRIVATE ${CMAKE_SOURCE_DIR}/include)
