cmake_minimum_required(VERSION 3.21)
project(autobus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_compile_options(-Wall -Wextra -pedantic -Werror)

enable_testing()
include(FetchContent)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()


add_subdirectory(src)
add_subdirectory(tests)


add_executable(autobus main.cpp)
target_include_directories(autobus PUBLIC autobusLib)
target_link_libraries(autobus PUBLIC autobusLib)


add_executable(autobusGui main_gui.cpp)
target_link_libraries(autobusGui PUBLIC autobusLibGui -static)
#set_target_properties( autobusGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../AppExe)

# hide console window
target_link_options(autobusGui PRIVATE -mwindows)