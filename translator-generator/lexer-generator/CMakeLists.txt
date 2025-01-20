find_package(BISON 3.2 REQUIRED)
find_package(FLEX REQUIRED)

flex_target(lexer src/InputLexicalAnalyzer.l
  ${CMAKE_CURRENT_BINARY_DIR}/InputLexicalAnalyzer.tab.cpp)
bison_target(parser src/InputParser.y
  ${CMAKE_CURRENT_BINARY_DIR}/InputParser.tab.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/InputParser.tab.h)
add_flex_bison_dependency(lexer parser)

add_library(LexerGenerator STATIC
  src/LexerGenerator.cpp
  ${FLEX_lexer_OUTPUTS}
  ${BISON_parser_OUTPUTS})
target_include_directories(LexerGenerator PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(LexerGenerator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_executable(trgpp-lexer src/main.cpp)
target_link_libraries(trgpp-lexer LexerGenerator)