cmake_minimum_required(VERSION 3.14)
project(expr-translator)

set(CMAKE_CXX_STANDARD 17)

# Book Lang
find_package(BISON 3.2 REQUIRED)
find_package(FLEX REQUIRED)

flex_target(lexer book/LexicalAnalyzer.l
  ${CMAKE_CURRENT_BINARY_DIR}/LexicalAnalyzer.tab.cpp)
bison_target(parser book/Parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/Parser.tab.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Parser.tab.h)
add_flex_bison_dependency(lexer parser)

add_library(book STATIC
  ${FLEX_lexer_OUTPUTS}
  ${BISON_parser_OUTPUTS})
target_include_directories(book PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(book PUBLIC book)

# Translator
add_executable(translator translator/main.cpp)
target_link_libraries(translator PRIVATE book)

# Tests
include(CTest)
include(FetchContent)
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(gtest)
link_libraries(book gtest gtest_main)

add_executable(ExpressionTests tests/ExpressionTests.cpp)
add_executable(StatementTests tests/StatementTests.cpp)
add_test(NAME expression COMMAND $<TARGET_FILE:ExpressionTests>)
add_test(NAME statement COMMAND $<TARGET_FILE:StatementTests>)