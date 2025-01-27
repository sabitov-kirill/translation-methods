add_executable(calculator src/main.cpp)
add_trgpp_target(calculator 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator.lex 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator.par)
target_link_libraries(calculator PRIVATE m)

add_executable(CalculatorBasicTests
  tests/BasicTests.cpp)
add_trgpp_target(CalculatorBasicTests 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator.lex 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator.par)
target_link_libraries(CalculatorBasicTests PRIVATE GTest::GTest GTest::Main m)
add_test(NAME CalculatorBasicTests COMMAND CalculatorBasicTests)