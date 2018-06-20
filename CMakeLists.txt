cmake_minimum_required(VERSION 3.6)
project(RaZ)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCC)
    set(GCC_FLAGS "-pedantic -pedantic-errors \
                   -Wall -Wextra \
                   -Wcast-align -Wcast-qual \
                   -Wdisabled-optimization \
                   -Wfloat-equal \
                   -Wformat-security -Wformat-signedness -Wformat=2 \
                   -Wmissing-declarations -Wmissing-include-dirs \
                   -Wnon-virtual-dtor \
                   -Wold-style-cast \
                   -Woverloaded-virtual \
                   -Wpacked \
                   -Wredundant-decls \
                   -Wundef -Wuninitialized \
                   -Wunused-macros \
                   -Wvla \
                   -Wconversion \
                   -Wswitch-default -Wswitch-enum \
                   -Wsuggest-attribute=noreturn \
                   -Wzero-as-null-pointer-constant \
                   -Wlogical-op \
                   -Wvector-operation-performance \
                   -Wdouble-promotion \
                   -Wtrampolines \
                   -Wuseless-cast \
                   -Wconditionally-supported \
                   -Wfloat-conversion \
                   -Wopenmp-simd \
                   -fsized-deallocation \
                   -Warray-bounds=2 \
                   -Wconditionally-supported \
                   -Wnoexcept \
                   -Wsized-deallocation \
                   -Wstrict-null-sentinel \
                   -Wsuggest-override \
                   -Wduplicated-cond \
                   -Wnull-dereference \
                   -Waligned-new \
                   -Walloc-zero -Walloca \
                   -Wformat-overflow \
                   -Wshadow=compatible-local \
                   -Wno-format-security -Wno-format-nonliteral \
                   -Wno-switch-default \
                   \
                   -Wno-old-style-cast \
                   -Wno-zero-as-null-pointer-constant \
                   -Wno-useless-cast \
                   -Wno-float-equal \
                   -Wno-conversion \
                   -Wno-missing-declarations \
                   -Wno-double-promotion \
                   -Wno-cast-qual") # Disabling some warnings triggered a lot by ImGui...

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CLANG_FLAGS "-pedantic -pedantic-errors \
                     -Weverything \
                     -Wno-c++98-compat -Wno-c++98-compat-pedantic \
                     -Wno-mismatched-tags \
                     -Wno-padded \
                     -Wno-shadow \
                     -Wno-global-constructors \
                     -Wno-weak-vtables \
                     -Wno-exit-time-destructors \
                     -Wno-covered-switch-default \
                     -Wno-unused-template \
                     -Wno-reserved-id-macro \
                     -Wno-documentation -Wno-documentation-unknown-command")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif ()

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
option(RAZ_BUILD_EXAMPLES "Build examples along RaZ" ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/png
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/deps
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

AUX_SOURCE_DIRECTORY(extern/png PNG_SRC)
AUX_SOURCE_DIRECTORY(extern/glew/src GLEW_SRC)
AUX_SOURCE_DIRECTORY(extern/glfw/src GLFW_SRC)
AUX_SOURCE_DIRECTORY(extern/imgui IMGUI_SRC)

set(
    SRC

    ${PNG_SRC}
    ${GLEW_SRC}
    ${GLFW_SRC}
    ${IMGUI_SRC}

    src/RaZ/Math/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/RaZ.hpp
    include/RaZ/Math/*.hpp
    include/RaZ/Math/*.inl
    include/RaZ/Render/*.hpp
    include/RaZ/Render/*.inl
    include/RaZ/Utils/*.hpp
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

        #-fsanitize=undefined
        #-fsanitize=address
        #-fsanitize=leak
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

    set(
        LIBS

        #asan
        #ubsan
    )

    set(
        DEPS

        X11
        GL
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
        DEPS

        opengl32
    )
endif ()

# Adding recursively every file we want to compile
file(
    GLOB
    SOURCE_FILES

    ${SRC}
)

# Removing unwanted platform-specific files
list(
    REMOVE_ITEM
    SOURCE_FILES

    ${DISPOSABLE}
)

if (${RAZ_BUILD_STATIC})
    add_library(RaZ STATIC ${SOURCE_FILES})
else ()
    add_library(RaZ SHARED ${SOURCE_FILES})
endif ()

target_link_libraries(
    RaZ ${LIBS}
    ${DEPS}
)

if (${RAZ_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif ()
