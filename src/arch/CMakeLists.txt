file(GLOB_RECURSE ARCH_LANG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(arch ${ARCH_LANG_SRC})

target_link_libraries(arch
  PRIVATE
	  arch_libcore
	  arch_libsyntax
)
