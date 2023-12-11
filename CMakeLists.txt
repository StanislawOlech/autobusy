cmake_minimum_required(VERSION 3.21)
project(autobus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_compile_options(-Wall -Wextra -pedantic -Werror)

enable_testing()
include(FetchContent)


add_subdirectory(src)
add_subdirectory(tests)


add_executable(autobus main.cpp)
target_include_directories(autobus PUBLIC autobusLib)
target_link_libraries(autobus PUBLIC autobusLib)
