cmake_minimum_required(VERSION 3.21)
project(autobus)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(autobus main.cpp)

add_executable(autobus2 main2.cpp)
set_target_properties(autobus2 PROPERTIES CMAKE_CXX_STANDARD 20)

