cmake_minimum_required(VERSION 3.28)
project(trgpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_subdirectory(location-api)
add_subdirectory(lexer-generator)

add_subdirectory(user)

add_subdirectory(examples)
