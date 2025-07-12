file(GLOB_RECURSE ARCH_LIBSYNTAX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(arch_libsyntax
  STATIC
	  ${ARCH_LIBSYNTAX_SRC}
)

target_include_directories(arch_libsyntax
  PUBLIC
	  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(arch_libsyntax
	PRIVATE
		arch_libcore
)
