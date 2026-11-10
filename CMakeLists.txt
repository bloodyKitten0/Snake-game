cmake_minimum_required(VERSION 3.20)

project(Snake LANGUAGES CXX)

add_executable(snake main.cpp)

target_compile_features(snake PRIVATE cxx_std_20)

target_compile_options(snake PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)