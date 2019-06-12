cmake_minimum_required(VERSION 3.6)
project(RaZ)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

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
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
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
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6)
        set(
            RAZ_COMPILER_FLAGS

            ${RAZ_COMPILER_FLAGS}
            -Wduplicated-cond
            -Wnull-dereference
        )
    endif ()

    # Enabling some other warnings available since GCC 7
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 7 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
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
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
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

include_directories(
    SYSTEM # The dependencies' headers are included as system ones, disabling all warnings

    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/png
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/deps
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

aux_source_directory(extern/png PNG_SRC)
aux_source_directory(extern/glew/src GLEW_SRC)
aux_source_directory(extern/glfw/src GLFW_SRC)
aux_source_directory(extern/imgui IMGUI_SRC)

set(
    RAZ_SRC

    ${PNG_SRC}
    ${GLEW_SRC}
    ${GLFW_SRC}
    ${IMGUI_SRC}

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

# Defining preprocessor macros and selecting files to be removed
if (APPLE)
    add_definitions(
        -D_GLFW_COCOA
        -D_GLFW_USE_CHDIR
        -D_GLFW_USE_MENUBAR
        -D_GLFW_USE_RETINA
    )

    file(
        GLOB
        DISPOSABLE

        extern/glfw/src/win32*
        extern/glfw/src/x11*
        extern/glfw/src/wl*
        extern/glfw/src/mir*
        extern/glfw/src/glx*
        extern/glfw/src/linux*
    )
elseif (UNIX)
    add_definitions(
        -D_GLFW_X11
        -D_GLFW_HAS_XF86VM
    )

    file(
        GLOB
        DISPOSABLE

        extern/glfw/src/cocoa*
        extern/glfw/src/win32*
        extern/glfw/src/wl*
        extern/glfw/src/wgl*
        extern/glfw/src/mir*
        extern/glfw/src/*.m
    )

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

    set(
        RAZ_LINKER_FLAGS

        ${RAZ_LINKER_FLAGS}
        GL
        X11
        Xrandr
        Xcursor
        Xinerama
        Xxf86vm
        pthread
        dl
    )
elseif (WIN32)
    add_definitions(
        -D_GLFW_WIN32
        -DGLEW_BUILD
        #-DGLEW_NO_GLU
    )

    file(
        GLOB
        DISPOSABLE

        extern/glfw/src/cocoa*
        extern/glfw/src/x11*
        extern/glfw/src/wl*
        extern/glfw/src/mir*
        extern/glfw/src/glx*
        extern/glfw/src/linux*
        extern/glfw/src/posix*
        extern/glfw/src/*.m
    )

    set(
        RAZ_LINKER_FLAGS

        ${RAZ_LINKER_FLAGS}
        opengl32
    )

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
            set(RAZ_LINKER_FLAGS ${RAZ_LINKER_FLAGS} ${FBX_LIB})
            message("FBX SDK linked.")
        endif ()
    endif ()
endif ()

# If FBX SDK not found, do not compile FBX loader
if (NOT RAZ_USE_FBX OR NOT FBX_FOUND)
    file(
        GLOB
        DISPOSABLE

        ${DISPOSABLE}
        src/RaZ/Utils/FbxImporter.cpp
    )
endif ()

# Adding recursively every file we want to compile
file(
    GLOB
    RAZ_FILES

    ${RAZ_SRC}
)

# Removing unwanted platform-specific files
list(
    REMOVE_ITEM
    RAZ_FILES

    ${DISPOSABLE}
)

###############
# RaZ - Build #
###############

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)

if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC ${RAZ_FILES})
else ()
    add_library(RaZ SHARED ${RAZ_FILES})
endif ()

if (NOT MSVC)
    # Defining the compiler flags only for C++; this doesn't work with MSVC
    set(RAZ_COMPILER_FLAGS $<$<COMPILE_LANGUAGE:CXX>:${RAZ_COMPILER_FLAGS}>)
endif ()

target_compile_options(RaZ PUBLIC ${RAZ_COMPILER_FLAGS})
target_link_libraries(RaZ ${RAZ_LINKER_FLAGS})

# Build the examples
option(RAZ_BUILD_EXAMPLES "Build examples along RaZ" ON)
if (RAZ_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Build the unit tests
option(RAZ_BUILD_TESTS "Compile the tests after RaZ is built" ON)
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
set(
    HEADERS_INSTALL

    ${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ
)

install(DIRECTORY ${HEADERS_INSTALL} DESTINATION include)
