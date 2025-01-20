project(translator-generator-user)

add_library(TRGPP INTERFACE)
target_include_directories(TRGPP INTERFACE include)
target_link_libraries(TRGPP INTERFACE LocationAPI)

