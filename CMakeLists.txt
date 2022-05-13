cmake_minimum_required(VERSION 3.14)
project(ccompiler C)


set (CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories("src/")

file(GLOB_RECURSE SRC_FILES "src/main.c" "src/*.h")

add_executable(ccompiler ${SRC_FILES})

install(TARGETS ccompiler RUNTIME DESTINATION bin)
SET(CMAKE_CXX_FLAGS -std=c11)
