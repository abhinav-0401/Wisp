cmake_minimum_required(VERSION 4.0)
project(Wisp)

set(CMAKE_CXX_STANDARD 23)

add_executable(Wisp
               Src/Interpreter.cpp
               Src/WispValue.h)

add_subdirectory(Src)
