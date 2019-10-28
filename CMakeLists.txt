cmake_minimum_required(VERSION 3.10)
project(RaZ)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##########################
# RaZ - Useful variables #
##########################

if (MSVC)
    set(RAZ_COMPILER_MSVC ON)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(RAZ_COMPILER_CLANG ON)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(RAZ_COMPILER_GCC ON)
endif ()

if (WIN32 OR CYGWIN)
    set(RAZ_PLATFORM_WINDOWS ON)
elseif (APPLE)
    set(RAZ_PLATFORM_MAC ON)
elseif (UNIX)
    set(RAZ_PLATFORM_LINUX ON)
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
        -Wswitch-default
        -Wswitch-enum
        -Wtrampolines
        -Wundef
        -Wuninitialized
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvla
        -Wzero-as-null-pointer-constant

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
            -Wsuggest-override
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
            -Wshadow=compatible-local
        )
    endif ()

    # Enabling code coverage
    option(RAZ_ENABLE_COVERAGE "Enable code coverage (GCC only)" OFF)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND RAZ_ENABLE_COVERAGE)
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

        -pedantic
        -pedantic-errors
        -Weverything

        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-mismatched-tags
        -Wno-missing-braces
        -Wno-padded
        -Wno-reserved-id-macro
        -Wno-shadow
        -Wno-weak-vtables
    )

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

        /W4
        /permissive-
    )

    # To automatically export all the classes & functions
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

######################
# RaZ - Source files #
######################

set(
    RAZ_SRC

    src/RaZ/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Physics/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/*.hpp
    include/RaZ/*.inl
    include/RaZ/Math/*.hpp
    include/RaZ/Math/*.inl
    include/RaZ/Physics/*.hpp
    include/RaZ/Render/*.hpp
    include/RaZ/Render/*.inl
    include/RaZ/Utils/*.hpp
    include/RaZ/Utils/*.inl
)

# Adding recursively every file we want to compile
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

#########################
# RaZ - FBX file format #
#########################

if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_GCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is considered as GCC
    option(RAZ_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" OFF)

    if (RAZ_USE_FBX)
        find_package(FBX)

        if (FBX_FOUND)
            set(RAZ_LINKER_FLAGS ${RAZ_LINKER_FLAGS} "${FBX_LIB}")

            message("--- FBX SDK linked")
        endif ()
    endif ()
endif ()

# If FBX SDK not found, do not compile FBX loader
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

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC ${RAZ_FILES})
else ()
    add_library(RaZ SHARED ${RAZ_FILES})
endif ()

target_include_directories(RaZ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# OpenGL version
option(RAZ_USE_GL4 "Use OpenGL 4" OFF)
if (RAZ_USE_GL4)
    target_compile_definitions(RaZ PRIVATE RAZ_USE_GL4)
endif ()

# FBX usage
if (FBX_FOUND)
    target_include_directories(RaZ PUBLIC "${FBX_INCLUDE}")
    target_compile_definitions(RaZ PRIVATE ${FBX_DEFS})
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

if (RAZ_USE_GLFW)
    target_link_libraries(RaZ PRIVATE GLFW)
endif ()

if (RAZ_USE_IMGUI)
    target_link_libraries(RaZ PRIVATE ImGui)
endif ()

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
