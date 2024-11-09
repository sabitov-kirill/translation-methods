cmake_minimum_required(VERSION 3.15)
project(rec-parser)

add_library(RecursiveParser STATIC parser/LexicalAnalyzer.cpp)
target_include_directories(RecursiveParser PUBLIC parser)

add_executable(Visualizer visualizer/main.cpp)
target_link_libraries(Visualizer PRIVATE cdt cgraph gvc)

include(CTest)
include(FetchContent)
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(gtest)

add_executable(LexerTests tests/LexerTests.cpp)
target_link_libraries(LexerTests PRIVATE RecursiveParser gtest gtest_main)
add_test(NAME basic_tokens COMMAND $<TARGET_FILE:LexerTests>)
