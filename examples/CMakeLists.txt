cmake_minimum_required(VERSION 3.6)
project(RaZ_Examples)

set(CMAKE_CXX_STANDARD 14)

add_executable(RaZ_fullDemo fullDemo.cpp)
target_link_libraries(RaZ_fullDemo RaZ)

add_executable(RaZ_minDemo minDemo.cpp)
target_link_libraries(RaZ_minDemo RaZ)
