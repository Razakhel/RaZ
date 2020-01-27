project(RaZ_Tests)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

set(
    RAZ_TESTS_COMPILER_FLAGS

    ${RAZ_COMPILER_FLAGS}
)

if (RAZ_COMPILER_CLANG)
    set(
        RAZ_TESTS_COMPILER_FLAGS

        ${RAZ_COMPILER_FLAGS}
        -Wno-double-promotion # [long] double/float operations are voluntarily made
        -Wno-float-equal # Floating-point strict equality checks are voluntarily made
        -Wno-header-hygiene # The namespace std::literals is imported in the Catch.hpp custom header
    )
endif ()

set(
    TESTS_SRC

    Main.cpp

    RaZ/*.cpp
    RaZ/Math/*.cpp
    RaZ/Render/*.cpp
    RaZ/Utils/*.cpp

    Catch/Catch.hpp
)

file(
    GLOB
    TESTS_FILES

    ${TESTS_SRC}
)

add_executable(RaZ_Tests ${TESTS_FILES})
target_include_directories(
    RaZ_Tests

    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/Catch"
    "${CMAKE_HOME_DIRECTORY}/extern"
)

# Adding a definition for the tests root path, so that it can be used in tests instead of full relative paths
target_compile_definitions(RaZ_Tests PRIVATE -DRAZ_TESTS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

# Disabling some warnings in tests
target_compile_options(RaZ_Tests PRIVATE ${RAZ_TESTS_COMPILER_FLAGS})

target_link_libraries(RaZ_Tests RaZ)

add_test(RaZ_Tests RaZ_Tests)

# If FBX SDK used with MSVC, copy the DLL into the tests/ folder to link it properly
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    add_custom_command(TARGET RaZ_Tests PRE_LINK
                       COMMAND ${CMAKE_COMMAND} -E copy "${FBX_CONFIG_DLL}" "${CMAKE_BINARY_DIR}/tests/${FBX_LIB_NAME}.dll")
endif ()
