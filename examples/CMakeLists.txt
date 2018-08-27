cmake_minimum_required(VERSION 3.6)
project(RaZ_Examples)

set(CMAKE_CXX_STANDARD 14)

# If FBX SDK used, copy the DLL file into the examples folder to link it properly
if (FBX_FOUND)
    if (MSVC)
        file(COPY "${FBX_ROOT_DIR}/${FBX_LIB_DIR}/libfbxsdk.dll" DESTINATION ${CMAKE_BINARY_DIR}/examples)
    endif ()
endif ()

add_executable(RaZ_fullDemo fullDemo.cpp)
target_link_libraries(RaZ_fullDemo RaZ)

add_executable(RaZ_minDemo minDemo.cpp)
target_link_libraries(RaZ_minDemo RaZ)

add_executable(RaZ_showcaseDemo showcaseDemo.cpp)
target_link_libraries(RaZ_showcaseDemo RaZ)
