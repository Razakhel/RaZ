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
add_compiler_flags(TARGET RaZ_Tests SCOPE PRIVATE)

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

        /wd4723 # Potential divide by 0

        # Warnings triggered by Catch
        /wd4388 # Signed/unsigned mismatch (equality comparison)
        /wd4583 # Destructor not implicitly called
        /wd4623 # Default constructor implicitly deleted
        /wd4868 # Evaluation order not guaranteed in braced initializing list
        /wd5219 # Implicit conversion, possible loss of data
        /wd5267 # Assignment operator implicitly deleted
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
    src/RaZ/Utils/*.cpp

    include/*.hpp
)

if (RAZ_USE_WINDOW OR RAZ_USE_EMSCRIPTEN)
    list(APPEND RAZ_TESTS_SRC src/RaZ/Render/*.cpp)
endif ()

if (RAZ_USE_LUA)
    list(APPEND RAZ_TESTS_SRC src/RaZ/Script/*.cpp)
endif ()

file(
    GLOB
    RAZ_TEST_FILES

    ${RAZ_TESTS_SRC}
)

############################
# RaZ Tests - Audio/OpenAL #
############################

if (RAZ_USE_AUDIO)
    file(
        GLOB
        RAZ_AUDIO_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Audio/*.cpp"
    )
    list(APPEND RAZ_TEST_FILES ${RAZ_AUDIO_TEST_FILES})

    if (WIN32)
        # Copying OpenAL's DLL to the tests folder
        file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/tests/OpenAL32.dll")
        file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/tests/libopenal-1.dll")
    endif ()
else ()
    list(
        REMOVE_ITEM
        RAZ_TEST_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/WavFormat.cpp"
        "${PROJECT_SOURCE_DIR}/src/RaZ/Script/LuaAudio.cpp"
    )
endif ()

###############################
# RaZ Tests - FBX file format #
###############################

if (NOT RAZ_USE_FBX)
    list(
        REMOVE_ITEM
        RAZ_TEST_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/FbxFormat.cpp"
    )
endif ()

#####################
# RaZ Tests - Build #
#####################

target_sources(RaZ_Tests PRIVATE ${RAZ_TEST_FILES})

target_include_directories(RaZ_Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Adding a definition for the tests root path, so that it can be used in tests instead of full relative paths
target_compile_definitions(RaZ_Tests PRIVATE RAZ_TESTS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

target_link_libraries(
    RaZ_Tests

    PRIVATE
        RaZ
        Catch2
)

if (WIN32 AND RAZ_USE_FBX)
    add_custom_command(
        TARGET RaZ_Tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:RaZ_Tests>
            $<TARGET_FILE_DIR:RaZ_Tests>
        COMMAND_EXPAND_LISTS
    )
endif ()

add_test(RaZ_Tests RaZ_Tests)
