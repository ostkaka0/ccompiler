cmake_minimum_required(VERSION 2.6)
project(ccompiler C)

#include_directories("src/")

file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.h")

add_executable(ccompiler ${SRC_FILES})

install(TARGETS ccompiler RUNTIME DESTINATION bin)
SET(CMAKE_CXX_FLAGS -std=c11)
