cmake_minimum_required(VERSION 3.5)
project(RaZ)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES RaZ/src/main.cpp)

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

#add_library(RaZ SHARED ${SOURCE_FILES})
add_executable(RaZ ${SOURCE_FILES})

#find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

if (WIN32)
    set(LIBS "opengl32 glfw3")
else ()
    set(LIBS "GL glfw")
endif ()

target_link_libraries(RaZ ${LIBS})
