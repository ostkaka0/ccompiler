cmake_minimum_required(VERSION 2.6)
project(ccompiler)

add_executable(ccompiler main.cpp)

install(TARGETS ccompiler RUNTIME DESTINATION bin)
SET(CMAKE_CXX_FLAGS -std=c++0x)