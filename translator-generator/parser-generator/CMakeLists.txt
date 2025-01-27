find_package(BISON 3.2 REQUIRED)
find_package(FLEX REQUIRED)

flex_target(PGInputLexer src/InputLexicalAnalyzer.l
  ${CMAKE_CURRENT_BINARY_DIR}/InputLexicalAnalyzer.tab.cpp)
bison_target(PGInputParser src/InputParser.y
  ${CMAKE_CURRENT_BINARY_DIR}/InputParser.tab.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/InputParser.tab.h
  COMPILE_FLAGS --debug)
add_flex_bison_dependency(PGInputLexer PGInputParser)

add_library(ParserGenerator STATIC
  src/ParserGenerator.cpp
  src/GrammarAnalyzer.cpp
  ${FLEX_PGInputLexer_OUTPUTS}
  ${BISON_PGInputParser_OUTPUTS})
target_include_directories(ParserGenerator PUBLIC src ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(ParserGenerator PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

add_executable(trgpp-pg src/main.cpp)
target_link_libraries(trgpp-pg ParserGenerator)

add_executable(GrammarFirstFollowTests 
  tests/GrammarFirstFollowTests.cpp
  src/GrammarAnalyzer.cpp)
target_link_libraries(GrammarFirstFollowTests 
  PRIVATE 
  GTest::GTest 
  GTest::Main
  ParserGenerator)
add_test(NAME GrammarFirstFollowTests COMMAND GrammarFirstFollowTests)

add_executable(GrammarChecksTests 
  tests/GrammarChecksTests.cpp
  src/GrammarAnalyzer.cpp)
target_link_libraries(GrammarChecksTests 
  PRIVATE 
  GTest::GTest 
  GTest::Main
  ParserGenerator)
add_test(NAME GrammarChecksTests COMMAND GrammarChecksTests)