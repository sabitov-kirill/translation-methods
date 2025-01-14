cmake_minimum_required(VERSION 3.14)
project(expr-translator)

find_package(BISON 3.2 REQUIRED)
find_package(FLEX REQUIRED)

add_executable(translator src/main.cpp)
