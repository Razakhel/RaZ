cmake_minimum_required(VERSION 3.14...3.30.5)

project(
    RaZ
    VERSION 0.0.1
    DESCRIPTION "Modern & multiplatform 3D game engine"
    HOMEPAGE_URL https://github.com/Razakhel/RaZ
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# If the build type hasn't been specified, defaulting it to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

#################
# RaZ - Library #
#################

# Disabling globally the C++20 modules scan. Maybe not the best idea, but is unneeded for now and is much easier than doing it per target
# Files mentioned in the `CXX_MODULES` file set of a target will always be scanned and should be the proper way of using modules
# If needed, can be replaced with `set_target_properties(TargetName PROPERTIES CXX_SCAN_FOR_MODULES OFF)` for each target
# See: https://cmake.org/cmake/help/latest/prop_tgt/CXX_SCAN_FOR_MODULES.html
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC)
else ()
    add_library(RaZ SHARED)
endif ()

# Using C++20
target_compile_features(RaZ PRIVATE cxx_std_20)

##########################
# RaZ - Useful variables #
##########################

# Detect whether Emscripten is being used
if (CMAKE_CXX_COMPILER MATCHES "/em\\+\\+.*$")
    set(RAZ_USE_EMSCRIPTEN ON)
else ()
    set(RAZ_USE_EMSCRIPTEN OFF)
endif ()

if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Finding exclusively MSVC, not clang-cl
    set(RAZ_COMPILER "MSVC")
    set(RAZ_COMPILER_MSVC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_MSVC)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (MSVC)
        # Using clang-cl, for which both MSVC & Clang are found
        set(RAZ_COMPILER "Clang-cl")
        set(RAZ_COMPILER_CLANG_CL ON)
    else ()
        set(RAZ_COMPILER "Clang")
    endif ()

    set(RAZ_COMPILER_CLANG ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(RAZ_COMPILER "GCC")
    set(RAZ_COMPILER_GCC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_GCC)
endif ()

if (WIN32 OR CYGWIN OR MSYS)
    set(RAZ_PLATFORM "Windows")
    set(RAZ_PLATFORM_WINDOWS ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_WINDOWS)

    if (CYGWIN)
        set(RAZ_PLATFORM "${RAZ_PLATFORM} (Cygwin)")
        set(RAZ_PLATFORM_CYGWIN ON)
        target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_CYGWIN)
    endif ()

    # MSYS is defined only if the generator is "MSYS Makefiles"
    if (MSYS)
        set(RAZ_PLATFORM "${RAZ_PLATFORM} (MSYS)")
        set(RAZ_PLATFORM_MSYS ON)
        target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_MSYS)
    endif ()
elseif (APPLE)
    set(RAZ_PLATFORM "macOS")
    set(RAZ_PLATFORM_MAC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_MAC)
elseif (RAZ_USE_EMSCRIPTEN)
    set(RAZ_PLATFORM "Emscripten")
    set(RAZ_PLATFORM_EMSCRIPTEN ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_EMSCRIPTEN USE_OPENGL_ES USE_WEBGL)
elseif (UNIX)
    set(RAZ_PLATFORM "Linux")
    set(RAZ_PLATFORM_LINUX ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_LINUX)
endif ()

if (RAZ_COMPILER_MSVC)
    set(RAZ_CONFIG_DEBUG   "$<IF:$<CONFIG:Debug>,ON,OFF>")
    set(RAZ_CONFIG_RELEASE "$<IF:$<CONFIG:Debug>,OFF,ON>")
    set(RAZ_CONFIG_SHORT   "$<IF:$<CONFIG:Debug>,Debug,Release>")

    target_compile_definitions(RaZ PUBLIC $<IF:$<CONFIG:Debug>,RAZ_CONFIG_DEBUG,RAZ_CONFIG_RELEASE>)

    set(CONFIG_STR_UPPER $<UPPER_CASE:$<CONFIG>>)
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(RAZ_CONFIG_DEBUG   ON)
        set(RAZ_CONFIG_RELEASE OFF)
        set(RAZ_CONFIG_SHORT   "Debug")

        target_compile_definitions(RaZ PUBLIC RAZ_CONFIG_DEBUG)
    else ()
        set(RAZ_CONFIG_DEBUG   OFF)
        set(RAZ_CONFIG_RELEASE ON)
        set(RAZ_CONFIG_SHORT   "Release")

        target_compile_definitions(RaZ PUBLIC RAZ_CONFIG_RELEASE)
    endif ()

    string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_STR_UPPER)
endif ()

option(ENABLE_DEBUG_INFO "Creates a debug target which prints useful values" OFF)

if (ENABLE_DEBUG_INFO)
    cmake_host_system_information(RESULT PLATFORM_RELEASE QUERY OS_RELEASE)
    cmake_host_system_information(RESULT PLATFORM_VERSION QUERY OS_VERSION)

    # Trimming potential leading & trailing space characters (happens for Windows' values)
    string(STRIP ${PLATFORM_RELEASE} PLATFORM_RELEASE)
    string(STRIP ${PLATFORM_VERSION} PLATFORM_VERSION)

    add_custom_target(
        RaZ_PrintDebugInfo

        ${CMAKE_COMMAND} -E echo "" &&
        ${CMAKE_COMMAND} -E echo "####################" &&
        ${CMAKE_COMMAND} -E echo "# [RaZ] Debug info #" &&
        ${CMAKE_COMMAND} -E echo "####################" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Platform (found ${RAZ_PLATFORM})" &&
        ${CMAKE_COMMAND} -E echo "  Name:     ${CMAKE_SYSTEM_NAME}" &&
        ${CMAKE_COMMAND} -E echo "  Release:  ${PLATFORM_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${PLATFORM_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Compiler (found ${RAZ_COMPILER})" &&
        ${CMAKE_COMMAND} -E echo "  ID:       ${CMAKE_CXX_COMPILER_ID}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${CMAKE_CXX_COMPILER_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "  Path:     ${CMAKE_CXX_COMPILER}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Configuration" &&
        ${CMAKE_COMMAND} -E echo "  Name:               $<CONFIG>" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_DEBUG:   ${RAZ_CONFIG_DEBUG}" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_RELEASE: ${RAZ_CONFIG_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_SHORT:   ${RAZ_CONFIG_SHORT}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Build flags: '${CMAKE_CXX_FLAGS_${CONFIG_STR_UPPER}}'" &&
        ${CMAKE_COMMAND} -E echo ""
    )
endif ()

if (RAZ_USE_EMSCRIPTEN)
    target_compile_definitions(RaZ PUBLIC RAZ_ROOT="/")
else ()
    target_compile_definitions(RaZ PUBLIC RAZ_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")
endif ()

########################
# RaZ - Compiler flags #
########################

option(RAZ_USE_SANITIZERS "Use compiler sanitizers" OFF)

if (RAZ_USE_SANITIZERS)
    set(SANITIZERS_OPTION USE_SANITIZERS)
endif ()

include(CompilerFlags)
add_compiler_flags(TARGET RaZ SCOPE PRIVATE ${SANITIZERS_OPTION})

if (RAZ_COMPILER_GCC)
    # Enabling code coverage
    option(RAZ_ENABLE_COVERAGE "Enable code coverage (GCC only)" OFF)

    if (RAZ_CONFIG_DEBUG AND RAZ_ENABLE_COVERAGE)
        target_compile_options(
            RaZ

            PRIVATE
                -g
                -O0
                -fno-inline
                -fno-inline-small-functions
                -fno-default-inline
                -fprofile-arcs
                -ftest-coverage
                -fprofile-update=atomic
        )

        target_link_libraries(RaZ PRIVATE gcov)
    endif ()
endif ()

if (RAZ_PLATFORM_LINUX)
    list(APPEND RAZ_LINKER_FLAGS pthread)
endif ()

if (RAZ_USE_EMSCRIPTEN)
    target_link_options(RaZ PUBLIC "SHELL:-s USE_GLFW=3")
    list(APPEND RAZ_LINKER_FLAGS glfw)
endif ()

######################
# RaZ - Source files #
######################

set(
    RAZ_SRC

    src/RaZ/*.cpp
    src/RaZ/Animation/*.cpp
    src/RaZ/Audio/*.cpp
    src/RaZ/Data/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Physics/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Script/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/*.hpp
    include/RaZ/*.inl
    include/RaZ/Animation/*.hpp
    include/RaZ/Audio/*.hpp
    include/RaZ/Audio/*.inl
    include/RaZ/Data/*.hpp
    include/RaZ/Data/*.inl
    include/RaZ/Math/*.hpp
    include/RaZ/Math/*.inl
    include/RaZ/Physics/*.hpp
    include/RaZ/Render/*.hpp
    include/RaZ/Render/*.inl
    include/RaZ/Script/*.hpp
    include/RaZ/Utils/*.hpp
    include/RaZ/Utils/*.inl
)

# Adding every file to be compiled
file(
    GLOB
    RAZ_FILES

    ${RAZ_SRC}
)

######################
# RaZ - Audio/OpenAL #
######################

find_package(OpenAL)

if (RAZ_USE_EMSCRIPTEN)
    # Emscripten's FindOpenAL.cmake script historically sets the capitalized found variable, but always finds it anyway
    set(OpenAL_FOUND ON)

    # Emscripten's OpenAL implementation does not (yet?) support audio effects
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Audio/SoundEffect.cpp"
        "${PROJECT_SOURCE_DIR}/src/RaZ/Audio/SoundEffectSlot.cpp"

        "${PROJECT_SOURCE_DIR}/include/RaZ/Audio/SoundEffect.hpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Audio/SoundEffectSlot.hpp"
    )
endif ()

option(RAZ_USE_AUDIO "Enable audio capabilities (requires OpenAL to be installed)" ${OpenAL_FOUND})

if (RAZ_USE_AUDIO)
    if (NOT OpenAL_FOUND)
        message(SEND_ERROR "Audio required but OpenAL not found; please review its location")
    endif ()

    if (NOT RAZ_USE_EMSCRIPTEN)
        # Emscripten manages OpenAL on its own; linking to it messes up predefined types
        target_link_libraries(RaZ PRIVATE OpenAL::OpenAL)
    endif ()
    target_compile_definitions(RaZ PUBLIC RAZ_USE_AUDIO)

    message(STATUS "[RaZ] Audio ENABLED")
else ()
    file(
        GLOB
        RAZ_AUDIO_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Audio/*"
        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/WavLoad.cpp"
        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/WavSave.cpp"
        "${PROJECT_SOURCE_DIR}/src/RaZ/Script/LuaAudio.cpp"

        "${PROJECT_SOURCE_DIR}/include/RaZ/Audio/*"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Data/WavFormat.hpp"
    )
    list(REMOVE_ITEM RAZ_FILES ${RAZ_AUDIO_FILES})

    message(STATUS "[RaZ] Audio DISABLED")
endif ()

#########################
# RaZ - FBX file format #
#########################

# Under Windows, Clang (not Clang-cl) uses Windows'/MSVC's libraries by default, and so should be compatible with the FBX SDK
# The SDK is unavailable with MinGW, which is considered as GCC
if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_CLANG OR (RAZ_COMPILER_GCC AND NOT MINGW))
    find_package(FBX)
    option(RAZ_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" ${FBX_FOUND})

    if (RAZ_USE_FBX)
        if (NOT FBX_FOUND)
            message(SEND_ERROR "FBX required but SDK not found; please review its location")
        endif ()

        target_link_libraries(RaZ PRIVATE FBX::FBX)
        target_compile_definitions(RaZ PUBLIC RAZ_USE_FBX)

        message(STATUS "[RaZ] FBX support ENABLED")
    else ()
        message(STATUS "[RaZ] FBX support DISABLED")
    endif ()
endif ()

# If FBX SDK not used or found, do not reference FBX files
if (NOT RAZ_USE_FBX OR NOT FBX_FOUND)
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Data/FbxLoad.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Data/FbxFormat.hpp"
    )
endif ()

#########################
# RaZ - Prepare shaders #
#########################

include(EmbedFiles)
embed_files(
    INPUT_PATTERN "${PROJECT_SOURCE_DIR}/shaders/*.*"
    OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/shaders"
    MAIN_TARGET RaZ
    EMBED_TARGET_SUFFIX Shaders
)

###############
# RaZ - Build #
###############

target_include_directories(RaZ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(SKIP_RENDERER_ERRORS "Do not print errors from the Renderer" OFF)
if (SKIP_RENDERER_ERRORS)
    target_compile_definitions(RaZ PRIVATE SKIP_RENDERER_ERRORS)
endif ()

option(RAZ_FORCE_DEBUG_LOG "Force the ouput of debug logging calls in non-Debug modes" OFF)
if (RAZ_FORCE_DEBUG_LOG)
    target_compile_definitions(RaZ PUBLIC RAZ_FORCE_DEBUG_LOG)
endif ()

target_link_libraries(RaZ PRIVATE ${RAZ_LINKER_FLAGS})

# Cygwin's Clang needs to use GCC's standard library
if (CYGWIN AND RAZ_COMPILER_CLANG)
    target_compile_options(RaZ PRIVATE -stdlib=libstdc++)
    target_link_libraries(RaZ PRIVATE stdc++)
endif ()

# Compiling & linking external libraries
add_subdirectory(extern)

if (RAZ_USE_GLEW)
    target_link_libraries(RaZ PRIVATE GLEW)
endif ()

if (NOT RAZ_USE_EMSCRIPTEN) # Emscripten handles GLFW on its own, but still needs the Window to render into
    if (RAZ_USE_WINDOW)
        target_link_libraries(RaZ PRIVATE GLFW)
        message(STATUS "[RaZ] Windowing ENABLED")
    else ()
        list(
            REMOVE_ITEM
            RAZ_FILES

            "${PROJECT_SOURCE_DIR}/src/RaZ/Render/Window.cpp"
            "${PROJECT_SOURCE_DIR}/src/RaZ/Script/LuaWindow.cpp"
            "${PROJECT_SOURCE_DIR}/include/RaZ/Render/Window.hpp"
        )

        target_compile_definitions(RaZ PUBLIC RAZ_NO_WINDOW)

        message(STATUS "[RaZ] Windowing DISABLED")
    endif ()
endif ()

if (RAZ_USE_IMGUI)
    target_link_libraries(RaZ PRIVATE ImGui)
    message(STATUS "[RaZ] Overlay ENABLED")
else ()
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Render/Overlay.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Render/Overlay.hpp"
    )

    target_compile_definitions(RaZ PUBLIC RAZ_NO_OVERLAY)

    message(STATUS "[RaZ] Overlay DISABLED")
endif ()

if (RAZ_USE_LUA)
    target_link_libraries(RaZ PRIVATE Sol2 Lua)
    message(STATUS "[RaZ] Lua scripting ENABLED")
else ()
    file(
        GLOB
        RAZ_LUA_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Script/*"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Script/*"
    )
    list(REMOVE_ITEM RAZ_FILES ${RAZ_LUA_FILES})

    target_compile_definitions(RaZ PUBLIC RAZ_NO_LUA)

    message(STATUS "[RaZ] Lua scripting DISABLED")
endif ()

if (NOT RAZ_PLATFORM_MAC AND NOT RAZ_USE_EMSCRIPTEN AND RAZ_USE_WINDOW)
    # XR currently isn't available with macOS or Emscripten and requires windowing capabilities
    # TODO: requiring windowing currently makes XR unavailable from the GUI editor, as it handles the windowing part on its own

    file(
        GLOB
        RAZ_XR_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/XR/*.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/XR/*.hpp"
    )
    list(APPEND RAZ_FILES ${RAZ_XR_FILES})

    target_link_libraries(RaZ PRIVATE OpenXR)
else ()
    list(REMOVE_ITEM RAZ_FILES "${PROJECT_SOURCE_DIR}/src/RaZ/Script/LuaXr.cpp")
endif ()

# The target is always linked, as the include files & their macros should be unconditionally available for ease of use; Tracy macros do something only
#  if the expected definition is available, which only is if the related CMake option is set
# It also always compiles the implementation file, but making it conditional would be much more of a hassle. This may change in the future
target_link_libraries(RaZ PUBLIC Tracy)

if (RAZ_USE_PROFILING)
    message(STATUS "[RaZ] Profiling ENABLED")
else ()
    message(STATUS "[RaZ] Profiling DISABLED")
    list(REMOVE_ITEM RAZ_FILES "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/Memory.cpp")
endif ()

target_link_libraries(RaZ PRIVATE fastgltf simdjson stb)

# Compiling RaZ's sources
target_sources(RaZ PRIVATE ${RAZ_FILES})

# Build the examples
option(RAZ_BUILD_EXAMPLES "Build examples" ON)
if (RAZ_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Build the unit tests
option(RAZ_BUILD_TESTS "Build unit tests" ON)
if (RAZ_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

# Allows to generate the documentation
find_package(Doxygen)
option(RAZ_GEN_DOC "Generate documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if (RAZ_GEN_DOC)
    add_subdirectory(doc)
endif ()

######################
# RaZ - Installation #
######################

# Installing the library
if (RAZ_PLATFORM_WINDOWS)
    set(CMAKE_INSTALL_PREFIX "C:/RaZ")
endif ()

install(
    TARGETS RaZ
    ARCHIVE DESTINATION "lib/$<CONFIG>"
    LIBRARY DESTINATION "lib/$<CONFIG>"
    RUNTIME DESTINATION "bin/$<CONFIG>"
)

# Installing the headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ" DESTINATION include)

# Installing DLL files into the bin/ folder
if (RAZ_COMPILER_MSVC AND RAZ_USE_AUDIO)
    install(FILES "${OPENAL_DLL}" RENAME "OpenAL32.dll"    DESTINATION "bin/$<CONFIG>")
    install(FILES "${OPENAL_DLL}" RENAME "libopenal-1.dll" DESTINATION "bin/$<CONFIG>")
endif ()
