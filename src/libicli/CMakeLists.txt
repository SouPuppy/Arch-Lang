add_library(arch_icli ./icli.cpp)

target_include_directories(arch_icli
  PRIVATE ${CMAKE_SOURCE_DIR}/include
)
