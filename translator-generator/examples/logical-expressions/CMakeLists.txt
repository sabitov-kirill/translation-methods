add_executable(logical-expressions src/main.cpp)
add_trgpp_target(logical-expressions
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logical-expressions.lex
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logical-expressions.par)
target_link_libraries(logical-expressions PRIVATE cdt cgraph gvc)
