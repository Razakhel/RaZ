cmake_minimum_required(VERSION 3.5)
project(RaZ)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES RaZ/src/main.cpp)

# Can be indicated like SHARED
add_library(RaZ ${SOURCE_FILES})