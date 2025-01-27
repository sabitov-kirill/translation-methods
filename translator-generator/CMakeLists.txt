cmake_minimum_required(VERSION 3.28)
project(trgpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
find_package(GTest REQUIRED)
enable_testing()

add_subdirectory(lexer-generator)
add_subdirectory(parser-generator)

add_subdirectory(user)

set(TRGPP_LEXER_PATH "${CMAKE_BINARY_DIR}/lexer-generator/trgpp-lg" 
  CACHE PATH "Path to trgpp-lexer executable")
set(TRGPP_PARSER_PATH "${CMAKE_BINARY_DIR}/parser-generator/trgpp-pg"
  CACHE PATH "Path to trgpp-parser executable")

function(add_trgpp_target T L P)
  set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/trgpp-${T}")
  set(LEXER_OUTPUT "${OUTPUT_DIR}/lexer.h")
  set(PARSER_OUTPUT "${OUTPUT_DIR}/parser.h")

  add_custom_command(
    OUTPUT ${LEXER_OUTPUT} ${PARSER_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
    COMMAND ${TRGPP_LEXER_PATH} ${L} ${LEXER_OUTPUT}
    COMMAND ${TRGPP_PARSER_PATH} ${P} ${PARSER_OUTPUT}
    DEPENDS ${L} ${P} ${TRGPP_LEXER_PATH} ${TRGPP_PARSER_PATH}
    COMMENT "Generating lexer and parser for ${T}"
  )

  add_custom_target(${T}_gen DEPENDS ${LEXER_OUTPUT} ${PARSER_OUTPUT})
  add_dependencies(${T} ${T}_gen)

  target_include_directories(${T} PRIVATE ${OUTPUT_DIR})
  target_link_libraries(${T} PRIVATE TRGPP)
endfunction()


add_subdirectory(examples/calculator)
