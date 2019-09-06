cmake_minimum_required(VERSION 3.10)
project(RaZ)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

########################
# RaZ - Compiler flags #
########################

if (CMAKE_COMPILER_IS_GNUCC)
    set(
        RAZ_COMPILER_FLAGS

        -pedantic
        -pedantic-errors
        -Wall
        -Wextra
        -Wcast-align
        -Wcast-qual
        -Wdisabled-optimization
        -Wfloat-equal
        -Wformat=2
        -Wformat-security
        -Wno-format-nonliteral
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpacked
        -Wredundant-decls
        -Wundef
        -Wuninitialized
        -Wunused-macros
        -Wvla
        -Wconversion
        -Wswitch-default
        -Wswitch-enum
        -Wzero-as-null-pointer-constant
        -Wlogical-op
        -Wvector-operation-performance
        -Wdouble-promotion
        -Wtrampolines
        -Wuseless-cast
        -Wconditionally-supported
        -Wfloat-conversion
        -Wopenmp-simd
        -Warray-bounds
        -Wnoexcept
        -Wstrict-null-sentinel

        # Disabling some warnings triggered a lot by ImGui...
        -Wno-old-style-cast
        -Wno-zero-as-null-pointer-constant
        -Wno-useless-cast
        -Wno-float-equal
        -Wno-conversion
        -Wno-missing-declarations
        -Wno-double-promotion
        -Wno-cast-qual
        -Wno-switch-default
    )

    # Enabling some other warnings available since GCC 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -Wformat-signedness
            -fsized-deallocation
            -Wsized-deallocation
            -Wsuggest-override
            -Warray-bounds=2
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
            -Walloc-zero
            -Walloca
            -Wformat-overflow
            -Wshadow=compatible-local
            -Waligned-new
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
            -fprofile-arcs
            -ftest-coverage
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
        )

        set(
            RAZ_LINKER_FLAGS

            gcov
        )
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(
        RAZ_COMPILER_FLAGS

        -pedantic
        -pedantic-errors
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-mismatched-tags
        -Wno-padded
        -Wno-shadow
        -Wno-global-constructors
        -Wno-weak-vtables
        -Wno-exit-time-destructors
        -Wno-covered-switch-default
        -Wno-reserved-id-macro
        -Wno-missing-braces
        -Wno-documentation
        -Wno-documentation-unknown-command
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

# Defining platform-dependent options & linking flags
if (UNIX AND NOT APPLE)
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

# Adding recursively every file we want to compile
file(
    GLOB
    RAZ_FILES

    ${RAZ_SRC}
)

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
