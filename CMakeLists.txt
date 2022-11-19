cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

project(verdant)

add_subdirectory(verdant)

add_executable(verdant_demo DemoMain.cpp)
target_link_libraries(verdant_demo verdant)
