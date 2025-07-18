file(GLOB_RECURSE ARCH_LANG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(archi ${ARCH_LANG_SRC})

target_link_libraries(archi
  PRIVATE
	  arch_libcore
	  arch_libsyntax
)
