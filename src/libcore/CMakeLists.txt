file(GLOB_RECURSE ARCH_LIBCORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(arch_libcore
  STATIC
	  ${ARCH_LIBCORE_SRC}
)

target_include_directories(arch_libcore
  PUBLIC
	  ${PROJECT_SOURCE_DIR}/include
)
