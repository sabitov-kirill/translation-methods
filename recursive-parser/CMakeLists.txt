cmake_minimum_required(VERSION 3.15)
project(rec-parser)

set(CMAKE_CXX_STANDARD 23)

add_library(RecursiveParser STATIC parser/SyntaxAnalyzer.cpp)
target_include_directories(RecursiveParser PUBLIC parser)

add_executable(Visualizer visualizer/main.cpp)
target_link_libraries(Visualizer PRIVATE RecursiveParser cdt cgraph gvc)

include(CTest)
include(FetchContent)
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(gtest)
link_libraries(RecursiveParser gtest gtest_main)

add_executable(LexerTests tests/LexerTests.cpp)
add_executable(SyntaxTests tests/SyntaxTests.cpp)
add_test(NAME lexer_tokens COMMAND $<TARGET_FILE:LexerTests>)
add_test(NAME syntax_tokens COMMAND $<TARGET_FILE:SyntaxTests>)
