cmake_minimum_required(VERSION 4.0)
project(Wisp)

set(CMAKE_CXX_STANDARD 20)

add_executable(Wisp
               Src/PrintVisitor.h
               Src/PrintVisitor.cpp)

add_subdirectory(Src)
