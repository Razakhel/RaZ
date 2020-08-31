cmake_minimum_required(VERSION 3.10)
project(RaZ)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# If the build type hasn't been specified, defaulting it to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

#################
# RaZ - Library #
#################

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC)
else ()
    add_library(RaZ SHARED)
endif ()

# Using C++17
target_compile_features(RaZ PRIVATE cxx_std_17)

##########################
# RaZ - Useful variables #
##########################

# Detect whether Emscripten is being used
if (CMAKE_CXX_COMPILER MATCHES "/em\\+\\+.*$")
    set(RAZ_USE_EMSCRIPTEN ON)
    set(CMAKE_AR "${CMAKE_C_COMPILER}")
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".bc")
    set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>")
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

if (WIN32 OR CYGWIN)
    set(RAZ_PLATFORM "Windows")
    set(RAZ_PLATFORM_WINDOWS ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_WINDOWS)

    if (CYGWIN)
        set(RAZ_PLATFORM "${RAZ_PLATFORM} (Cygwin)")
        set(RAZ_PLATFORM_CYGWIN ON)
        target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_CYGWIN)
    endif ()
elseif (APPLE)
    set(RAZ_PLATFORM "macOS")
    set(RAZ_PLATFORM_MAC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_MAC)
elseif (RAZ_USE_EMSCRIPTEN)
    set(RAZ_PLATFORM "Emscripten")
    set(RAZ_PLATFORM_EMSCRIPTEN ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_EMSCRIPTEN USE_OPENGL_ES)
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

set(RAZ_BUILD_FLAGS "${CMAKE_CXX_FLAGS_${CONFIG_STR_UPPER}}")

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

        ${CMAKE_COMMAND} -E echo "--- Build flags: '${RAZ_BUILD_FLAGS}'" &&
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

if (RAZ_COMPILER_GCC)
    set(
        RAZ_COMPILER_FLAGS

        -pedantic
        -pedantic-errors
        -Wall
        -Wextra

        -Warray-bounds
        -Wcast-align
        -Wcast-qual
        -Wconditionally-supported
        -Wconversion
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wfloat-conversion
        -Wformat=2
        -Wformat-security
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnoexcept
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wopenmp-simd
        -Woverloaded-virtual
        -Wpacked
        -Wredundant-decls
        -Wstrict-aliasing
        -Wstrict-null-sentinel
        #-Wsuggest-final-methods
        #-Wsuggest-final-types
        -Wtrampolines
        -Wundef
        -Wuninitialized
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvla
        -Wzero-as-null-pointer-constant

        -Wno-comment
        -Wno-format-nonliteral
    )

    # Enabling some other warnings available since GCC 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -fsized-deallocation
            -Warray-bounds=2
            -Wformat-signedness
            -Wsized-deallocation
        )
    endif ()

    # Enabling some other warnings available since GCC 6
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -Wduplicated-cond
            -Wnull-dereference
        )
    endif ()

    # Enabling some other warnings available since GCC 7
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -Waligned-new
            -Walloca
            -Walloc-zero
            -Wformat-overflow
            -Wshadow
        )
    endif ()

    # Enabling code coverage
    option(RAZ_ENABLE_COVERAGE "Enable code coverage (GCC only)" OFF)

    if (RAZ_CONFIG_DEBUG AND RAZ_ENABLE_COVERAGE)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -g
            -O0
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
            -fprofile-arcs
            -ftest-coverage
        )

        set(
            RAZ_LINKER_FLAGS

            gcov
        )
    endif ()
elseif (RAZ_COMPILER_CLANG)
    set(
        RAZ_COMPILER_FLAGS

        -Weverything

        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-exit-time-destructors
        -Wno-float-equal
        -Wno-format-nonliteral
        -Wno-global-constructors
        -Wno-mismatched-tags
        -Wno-missing-braces
        -Wno-padded
        -Wno-reserved-id-macro
        -Wno-sign-conversion
        -Wno-switch-enum
        -Wno-weak-vtables
    )

    if (RAZ_COMPILER_CLANG_CL)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            # Disabling warnings triggered in externals
            -Wno-language-extension-token
            -Wno-nonportable-system-include-path
            -Wno-zero-as-null-pointer-constant
        )
    else ()
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            # Other warning flags not recognized by clang-cl
            -pedantic
            -pedantic-errors
        )
    endif ()

    if (RAZ_USE_EMSCRIPTEN)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            "SHELL:-s USE_GLFW=3"
            "SHELL:-s USE_LIBPNG=1"
        )

        set(
            RAZ_LINKER_FLAGS

            ${RAZ_LINKER_FLAGS}
            "-s USE_LIBPNG=1"
            "-s USE_GLFW=3"
            "-s USE_WEBGL2=1"
            "-s OFFSCREEN_FRAMEBUFFER=1"
            "-s OFFSCREENCANVAS_SUPPORT=1"
            "-s DOUBLE_MODE=0"
            glfw
        )
    endif ()

    # Disabling some warnings available since Clang 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -Wno-unused-template
        )
    endif ()
elseif (RAZ_COMPILER_MSVC)
    set(
        RAZ_COMPILER_FLAGS

        /Wall

        /wd4061 # Enum value in a switch not explicitly handled by a case label
        /wd4571 # SEH exceptions aren't caught since Visual C++ 7.1
        /wd5045 # Spectre mitigation

        # Warnings triggered by the FBX SDK
        /wd4266 # No override available (function is hidden)
        /wd4619 # Unknown warning number
        /wd4625 # Copy constructor implicitly deleted
        /wd4626 # Copy assignment operator implicitly deleted

        # Warnings triggered by Catch
        /wd4365 # Signed/unsigned mismatch (implicit conversion)
        /wd4388 # Signed/unsigned mismatch (equality comparison)
        /wd4583 # Destructor not implicitly called
        /wd4623 # Default constructor implicitly deleted
        /wd4868 # Evaluation order not guaranteed in braced initializing list
        /wd5204 # Class with virtual functions but no virtual destructor

        # Warnings triggered by MSVC's standard library
        /wd4355 # 'this' used in base member initializing list
        /wd4514 # Unreferenced inline function has been removed
        /wd4548 # Expression before comma has no effect
        /wd4668 # Preprocessor macro not defined
        /wd4710 # Function not inlined
        /wd4711 # Function inlined
        /wd4774 # Format string is not a string literal
        /wd4820 # Added padding to members
        /wd5026 # Move constructor implicitly deleted
        /wd5027 # Move assignment operator implicitly deleted
        /wd5039 # Pointer/ref to a potentially throwing function passed to an 'extern "C"' function (with -EHc)
    )

    # To automatically export all the classes & functions
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    # CMake automatically appends /W3 to the standard flags, which produces a warning with MSVC when adding another level; this has to be removed
    # TODO: if possible, this should be done per target, not globally
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_CLANG_CL)
    set(
        RAZ_COMPILER_FLAGS

        ${RAZ_COMPILER_FLAGS}
        /permissive- # Improving standard compliance
        /EHsc # Enabling exceptions
        /utf-8 # Forcing MSVC to actually handle files as UTF-8
    )

    target_compile_definitions(
        RaZ

        PRIVATE

        NOMINMAX # Preventing definitions of min & max macros
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
    )
endif ()

######################
# RaZ - Source files #
######################

set(
    RAZ_SRC

    src/RaZ/*.cpp
    src/RaZ/Audio/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Physics/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/*.hpp
    include/RaZ/*.inl
    include/RaZ/Audio/*.hpp
    include/RaZ/Audio/*.inl
    include/RaZ/Math/*.hpp
    include/RaZ/Math/*.inl
    include/RaZ/Physics/*.hpp
    include/RaZ/Render/*.hpp
    include/RaZ/Render/*.inl
    include/RaZ/Utils/*.hpp
    include/RaZ/Utils/*.inl
)

# Adding every file to be compiled
file(
    GLOB
    RAZ_FILES

    ${RAZ_SRC}
)

# Defining platform-dependent options & linking flags
if (RAZ_PLATFORM_LINUX)
    option(RAZ_USE_SANITIZERS "Use sanitizers" OFF)

    if (RAZ_USE_SANITIZERS)
        add_definitions(
            -fsanitize=undefined
            -fsanitize=address
            -fsanitize=leak
        )

        set(
            RAZ_LINKER_FLAGS

            ${RAZ_LINKER_FLAGS}
            asan
            ubsan
        )
    endif ()
endif ()

################
# RaZ - OpenAL #
################

find_package(OpenAL REQUIRED)

target_include_directories(RaZ SYSTEM PUBLIC ${OPENAL_INCLUDE_DIRS})
target_link_libraries(RaZ PUBLIC ${OPENAL_LIBS})

#########################
# RaZ - FBX file format #
#########################

if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_GCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is considered as GCC
    option(RAZ_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" OFF)

    if (RAZ_USE_FBX)
        find_package(FBX)

        if (FBX_FOUND)
            set(RAZ_LINKER_FLAGS ${RAZ_LINKER_FLAGS} "${FBX_LIB}")

            target_compile_definitions(RaZ PUBLIC ${FBX_DEFINITIONS})
            target_include_directories(RaZ PUBLIC "${FBX_INCLUDE}")

            message(STATUS "[RaZ] FBX SDK linked (${FBX_LIB})")
        endif ()
    endif ()
endif ()

# If FBX SDK not used or found, do not compile FBX loader
if (NOT RAZ_USE_FBX OR NOT FBX_FOUND)
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/FbxImporter.cpp"
    )
endif ()

###############
# RaZ - Build #
###############

target_include_directories(RaZ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(SKIP_RENDERER_ERRORS "Do not print errors from the Renderer" OFF)
if (SKIP_RENDERER_ERRORS)
    target_compile_definitions(RaZ PRIVATE SKIP_RENDERER_ERRORS)
endif ()

# OpenGL version
option(RAZ_USE_GL4 "Use OpenGL 4" OFF)
if (RAZ_USE_GL4)
    target_compile_definitions(RaZ PRIVATE RAZ_USE_GL4)
endif ()

if (NOT RAZ_COMPILER_MSVC)
    # Defining the compiler flags only for C++; this doesn't work with MSVC
    set(RAZ_COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:${RAZ_COMPILER_FLAGS}>)
endif ()

# Compiling RaZ
target_compile_options(RaZ PRIVATE ${RAZ_COMPILER_FLAGS})
target_link_libraries(RaZ PRIVATE ${RAZ_LINKER_FLAGS})

# Cygwin's Clang needs to use GCC's standard library
if (CYGWIN AND RAZ_COMPILER_CLANG)
    target_compile_options(RaZ PRIVATE -stdlib=libstdc++)
    target_link_libraries(RaZ PRIVATE stdc++)
endif ()

# Compiling & linking external libraries
add_subdirectory(extern)

if (RAZ_USE_LIBPNG)
    target_link_libraries(RaZ PRIVATE libpng)
endif ()

if (RAZ_USE_GLEW)
    target_link_libraries(RaZ PRIVATE GLEW)
endif ()

if (RAZ_USE_GLFW OR RAZ_USE_EMSCRIPTEN)
    if (NOT RAZ_USE_EMSCRIPTEN)
        # Emscripten handles GLFW on its own, but it is however not available directly; its link must be disabled
        target_link_libraries(RaZ PRIVATE GLFW)
    endif ()

    target_compile_definitions(RaZ PUBLIC RAZ_USE_WINDOW)
else ()
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/Window.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Utils/Window.hpp"
    )
endif ()

if (RAZ_USE_IMGUI)
    target_link_libraries(RaZ PRIVATE ImGui)
    target_compile_definitions(RaZ PUBLIC RAZ_USE_OVERLAY)
else ()
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/Overlay.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Utils/Overlay.cpp"
    )
endif ()

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

install(TARGETS RaZ DESTINATION "lib/$<CONFIG>")

# Installing the headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ" DESTINATION include)

# If FBX SDK used, install the DLL file into the bin/ folder
if (RAZ_COMPILER_MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    install(FILES "${FBX_CONFIG_DLL}" DESTINATION "bin/$<CONFIG>")
endif ()
