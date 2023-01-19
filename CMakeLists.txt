cmake_minimum_required(VERSION 3.12)

project(
        lab4
        VERSION 1.0
        DESCRIPTION "C++ Year 2022 Lab3"
        LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(bin)