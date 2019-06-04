cmake_minimum_required(VERSION 3.6)
project(RaZ_Examples)

set(CMAKE_CXX_STANDARD 17)

# If FBX SDK used, copy the DLL file into the examples folder to link it properly
if (RAZ_USE_FBX AND FBX_FOUND)
    if (MSVC)
        file(COPY "${FBX_ROOT_DIR}/${FBX_LIB_DIR}/libfbxsdk.dll" DESTINATION "${CMAKE_BINARY_DIR}/examples")
    endif ()
endif ()

add_executable(RaZ_FullDemo fullDemo.cpp)
target_link_libraries(RaZ_FullDemo RaZ)

add_executable(RaZ_MinDemo minDemo.cpp)
target_link_libraries(RaZ_MinDemo RaZ)

add_executable(RaZ_ShowcaseDemo showcaseDemo.cpp)
target_link_libraries(RaZ_ShowcaseDemo RaZ)
