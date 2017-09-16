cmake_minimum_required(VERSION 3.6)
project(RaZ)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-value")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif ()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(
    SRC

    main.cpp

    src/RaZ/Mesh/Mesh.cpp
    src/RaZ/Mesh/Shader.cpp
    src/RaZ/Utils/Window.cpp

    include/RaZ/RaZ.hpp
    include/RaZ/Mesh/Mesh.hpp
    include/RaZ/Mesh/Shader.hpp
    include/RaZ/Utils/Window.hpp
)

# Disabling useless stuff from GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if (WIN32)
    set(GLEW_ROOT "D:\\Librairies\\glew-2.0.0")
    set(GLEW_INCLUDE_DIR "${GLEW_ROOT}\\include")
    include_directories(${GLEW_INCLUDE_DIR})
    set(GLEW_DIR "${GLEW_ROOT}\\bin\\Release\\x64")
    set(GLEW_LIBRARY "${GLEW_ROOT}\\lib\\Release\\x64")

    set(glfw3_ROOT "D:\\Program Files\\GLFW")
    include_directories(${glfw3_ROOT}\\include)
    set(glfw3_DIR "${glfw3_ROOT}\\lib\\cmake\\glfw3")
endif ()

#add_library(RaZ SHARED ${SRC})
add_executable(RaZ ${SRC})

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

if (WIN32)
    set(LIBS "opengl32 glfw")
else ()
    set(LIBS "-lGL -lglfw -lGLEW")
endif ()

target_link_libraries(RaZ ${LIBS})
