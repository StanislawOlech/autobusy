cmake_minimum_required(VERSION 3.21)
project(autobus)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(autobus main.cpp)

set_target_properties(autobus PROPERTIES CMAKE_CXX_STANDARD 20)

