project(RaZ_Tests)

##########################
# RaZ Tests - Executable #
##########################

add_executable(RaZ_Tests)

# Using C++17
target_compile_features(RaZ_Tests PRIVATE cxx_std_17)

enable_testing()

##############################
# RaZ Tests - Compiler flags #
##############################

set(
    RAZ_TESTS_COMPILER_FLAGS

    ${RAZ_COMPILER_FLAGS}
)

if (RAZ_COMPILER_CLANG)
    set(
        RAZ_TESTS_COMPILER_FLAGS

        ${RAZ_TESTS_COMPILER_FLAGS}
        -Wno-double-promotion # [long] double/float operations are voluntarily made
        -Wno-float-equal # Floating-point strict equality checks are voluntarily made
        -Wno-header-hygiene # The namespace std::literals is imported in the Catch.hpp custom header
        -Wno-unneeded-member-function # Test structures may contain unnecessary member functions
    )
endif ()

if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_CLANG_CL)
    set(
        RAZ_TESTS_COMPILER_FLAGS

        ${RAZ_TESTS_COMPILER_FLAGS}
        /permissive- # Improving standard compliance
        /EHsc # Enabling exceptions
        /utf-8 # Forcing MSVC to actually handle files as UTF-8
    )

    target_compile_definitions(
        RaZ_Tests

        PRIVATE

        NOMINMAX # Preventing definitions of min & max macros
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
    )
endif ()

############################
# RaZ Tests - Source files #
############################

set(
    RAZ_TESTS_SRC

    Main.cpp

    RaZ/*.cpp
    RaZ/Animation/*.cpp
    RaZ/Audio/*.cpp
    RaZ/Math/*.cpp
    RaZ/Physics/*.cpp
    RaZ/Render/*.cpp
    RaZ/Utils/*.cpp

    Catch/Catch.hpp
)

file(
    GLOB
    RAZ_TESTS_FILES

    ${RAZ_TESTS_SRC}
)

#####################
# RaZ Tests - Build #
#####################

target_sources(RaZ_Tests PRIVATE ${RAZ_TESTS_FILES})

target_include_directories(
    RaZ_Tests

    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/Catch"
    "${CMAKE_HOME_DIRECTORY}/extern"
)

# Adding a definition for the tests root path, so that it can be used in tests instead of full relative paths
target_compile_definitions(RaZ_Tests PRIVATE RAZ_TESTS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

target_compile_options(RaZ_Tests PRIVATE ${RAZ_TESTS_COMPILER_FLAGS})

target_link_libraries(RaZ_Tests PUBLIC RaZ)

add_test(RaZ_Tests RaZ_Tests)

###############################
# RaZ Tests - FBX file format #
###############################

# If FBX SDK used with MSVC, copy the DLL into the tests/ folder to link it properly
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    add_custom_command(TARGET RaZ_Tests PRE_LINK
                       COMMAND ${CMAKE_COMMAND} -E copy "${FBX_CONFIG_DLL}" "${CMAKE_BINARY_DIR}/tests/${FBX_LIB_NAME}.dll")
endif ()
