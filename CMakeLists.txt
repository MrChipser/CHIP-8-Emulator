cmake_minimum_required(VERSION 4.0)
project(chip8)

set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)

add_executable(chip8 main.cpp
        chip8.hpp
        chip8.cpp
        platform.cpp
        platform.hpp)

target_link_libraries(chip8 PRIVATE SDL2::SDL2)
