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

include(CompilerFlags)
add_compiler_flags(RaZ_Tests PRIVATE)

if (RAZ_COMPILER_CLANG)
    target_compile_options(
        RaZ_Tests

        PRIVATE

        -Wno-double-promotion # [long] double/float operations are voluntarily made
        -Wno-float-conversion # Can occur voluntarily for readability
        -Wno-implicit-int-float-conversion # Can occur voluntarily for readability
        -Wno-unneeded-member-function # Test structures may contain unnecessary member functions
    )

    # Disabling some other warnings available since Clang 13.X
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13 AND NOT APPLE)
        target_compile_options(RaZ_Tests PRIVATE -Wno-reserved-identifier) # Each Catch test case macro triggers this
    endif ()
elseif (RAZ_COMPILER_MSVC)
    target_compile_options(
        RaZ_Tests

        PRIVATE

        # Warnings triggered by Catch
        /wd4388 # Signed/unsigned mismatch (equality comparison)
        /wd4583 # Destructor not implicitly called
        /wd4623 # Default constructor implicitly deleted
        /wd4868 # Evaluation order not guaranteed in braced initializing list
        /wd5219 # Implicit conversion, possible loss of data
    )
endif ()

############################
# RaZ Tests - Source files #
############################

set(
    RAZ_TESTS_SRC

    Main.cpp

    src/*.cpp
    src/RaZ/*.cpp
    src/RaZ/Animation/*.cpp
    src/RaZ/Data/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Physics/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/*.hpp
)

if (RAZ_USE_AUDIO)
    list(APPEND RAZ_TESTS_SRC src/RaZ/Audio/*.cpp)
endif ()

if (RAZ_USE_SOL2)
    list(APPEND RAZ_TESTS_SRC src/RaZ/Script/*.cpp)
endif ()

file(
    GLOB
    RAZ_TESTS_FILES

    ${RAZ_TESTS_SRC}
)

###############################
# RaZ Tests - FBX file format #
###############################

# If FBX SDK used with MSVC, copy the DLL into the tests/ folder to link it properly
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    file(COPY "${FBX_DLL}" DESTINATION "${CMAKE_BINARY_DIR}/tests/")
else ()
    list(
        REMOVE_ITEM
        RAZ_TESTS_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/FbxFormat.cpp"
    )
endif ()

#####################
# RaZ Tests - Build #
#####################

target_sources(RaZ_Tests PRIVATE ${RAZ_TESTS_FILES})

target_include_directories(
    RaZ_Tests

    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_HOME_DIRECTORY}/extern"
)

# Adding a definition for the tests root path, so that it can be used in tests instead of full relative paths
target_compile_definitions(RaZ_Tests PRIVATE RAZ_TESTS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(RaZ_Tests PUBLIC RaZ)

add_test(RaZ_Tests RaZ_Tests)
