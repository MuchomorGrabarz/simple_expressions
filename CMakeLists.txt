cmake_minimum_required(VERSION 3.10)

project(expressions)

add_subdirectory(deps/hive/libraries/fc)

add_executable(simple_expressions main.cpp)
set_property(TARGET simple_expressions PROPERTY CXX_STANDARD 17)

target_include_directories(simple_expressions PRIVATE deps/hive/libraries/fc/include)

target_link_libraries(simple_expressions PRIVATE fc)

