add_library(LocationAPI INTERFACE)
target_include_directories(LocationAPI INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
