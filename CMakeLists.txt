cmake_minimum_required(VERSION 4.0)
project(Wisp)

set(CMAKE_CXX_STANDARD 20)

add_executable(Wisp
               Src/Parser.h
               Src/Parser.cpp
               Src/AST.h)

add_subdirectory(Src)
