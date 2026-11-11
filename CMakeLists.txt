
cmake_minimum_required(VERSION 3.20)

project(SnakeGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_logic INTERFACE)

target_include_directories(game_logic INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(snake main.cpp)
target_link_libraries(snake PRIVATE game_logic)

add_executable(snake_tests tests.cpp)
target_link_libraries(snake_tests PRIVATE game_logic)

enable_testing()
add_test(NAME SnakeTests COMMAND snake_tests)
