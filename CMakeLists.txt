cmake_minimum_required(VERSION 3.10)
project(RaZ)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

########################
# RaZ - Compiler flags #
########################

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
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

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" AND RAZ_ENABLE_COVERAGE)
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
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
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
elseif (MSVC)
    set(
        RAZ_COMPILER_FLAGS

        /W4

        # Disabling some warnings triggered by compiling files from external libraries
        /wd4100 # [GLFW] Unreferenced formal parameter
        /wd4127 # [libpng] Conditional expression is constant
        /wd4131 # [libpng] Uses old-style declarator
        /wd4152 # [GLFW] Non-standard extension (function/data pointer conversion in expression)
        /wd4204 # [GLFW] Non-standard extension (non-constant aggregate initializer)
        /wd4244 # [libpng, GLFW] Type narrowing from T (possible loss of data)
        /wd4245 # [libpng] Signed/unsigned conversion
        /wd4267 # [libpng] Type narrowing from size_t (possible loss of data)
        /wd4456 # [GLEW] Shadowing (declaration of 'var' hides previous local declaration)
        /wd4996 # [libpng, GLFW] Deprecated declaration
    )
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
if (UNIX AND NOT APPLE) # Linux
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
elseif (WIN32)
    # To automatically export all the classes & functions
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

#########################
# RaZ - FBX file format #
#########################

if (MSVC OR CMAKE_COMPILER_IS_GNUCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is triggered by IS_GNUCC
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

if (NOT MSVC)
    # Defining the compiler flags only for C++; this doesn't work with MSVC
    set(RAZ_COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:${RAZ_COMPILER_FLAGS}>)
endif ()

# Compiling RaZ
target_compile_options(RaZ PUBLIC ${RAZ_COMPILER_FLAGS})
target_link_libraries(RaZ ${RAZ_LINKER_FLAGS})

# Compiling external libraries
add_subdirectory(extern)
target_link_libraries(RaZ RaZ_Dependencies)

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
if (WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/RaZ")
endif ()

install(TARGETS RaZ DESTINATION "lib/$<CONFIG>")

# Installing the headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ" DESTINATION include)

# If FBX SDK used, install the DLL file into the bin/ folder
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    install(FILES "${FBX_CONFIG_DLL}" DESTINATION "bin/$<CONFIG>")
endif ()
