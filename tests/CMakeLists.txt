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

# If FBX SDK used, copy the DLL file into the tests folder to link it properly
if (RAZ_USE_FBX AND FBX_FOUND)
    if (MSVC)
        file(COPY "${FBX_ROOT_DIR}/${FBX_LIB_DIR}/libfbxsdk.dll" DESTINATION "${CMAKE_BINARY_DIR}/tests")
    endif ()
endif ()

add_executable(RaZ_Tests ${TESTS_FILES})
target_link_libraries(RaZ_Tests RaZ)
add_test(RaZ_Tests RaZ_Tests)
