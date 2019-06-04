cmake_minimum_required(VERSION 3.6)
project(RaZ_Tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extern/catch)

enable_testing()

set(
    TESTS_SRC

    Main.cpp

    RaZ/*.cpp
    RaZ/Math/*.cpp
    RaZ/Render/*.cpp
    RaZ/Utils/*.cpp
)

file(
    GLOB
    TESTS_FILES

    ${TESTS_SRC}
)

add_executable(RaZ_Tests ${TESTS_FILES})
target_link_libraries(RaZ_Tests RaZ)
add_test(RaZ_Tests RaZ_Tests)
