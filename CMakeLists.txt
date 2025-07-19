cmake_minimum_required(VERSION 3.25)

project(arch-lang)

# Version
set(ARCH_VERSION_MAJOR 0)
set(ARCH_VERSION_MINOR 1)
set(ARCH_VERSION_PATCH 0)
set(ARCH_VERSION "${ARCH_VERSION_MAJOR}.${ARCH_VERSION_MINOR}.${ARCH_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(./src/core)
add_subdirectory(./src/syntax)
add_subdirectory(./src/libicli)

add_subdirectory(./src/arch)	# arch toolchain
add_subdirectory(./src/archi) # arch cli

add_subdirectory(./example)

# version control
execute_process(
	COMMAND git rev-parse --short HEAD
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE ARCH_COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND git log -1 --format=%cd --date=short
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE ARCH_COMMIT_DATE
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
	${CMAKE_SOURCE_DIR}/config/version.h.in
	${CMAKE_BINARY_DIR}/include/arch/version.h
	@ONLY
)

# installation
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/modules
  DESTINATION .
)
