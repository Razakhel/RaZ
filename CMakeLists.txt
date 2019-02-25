cmake_minimum_required(VERSION 3.6)
project(RaZ)

set(CMAKE_CXX_STANDARD 14)

# Define build type to Release if not automatically set
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    set(
        GCC_FLAGS

        "-pedantic -pedantic-errors \
         -Wall -Wextra \
         -Wcast-align -Wcast-qual \
         -Wdisabled-optimization \
         -Wfloat-equal \
         -Wformat-security -Wformat=2 \
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
         -Wzero-as-null-pointer-constant \
         -Wlogical-op \
         -Wvector-operation-performance \
         -Wdouble-promotion \
         -Wtrampolines \
         -Wuseless-cast \
         -Wconditionally-supported \
         -Wfloat-conversion \
         -Wopenmp-simd \
         -Warray-bounds \
         -Wconditionally-supported \
         -Wnoexcept \
         -Wstrict-null-sentinel \
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
         -Wno-cast-qual" # Disabling some warnings triggered a lot by ImGui...
    )

    # Enabling some other warnings available since GCC 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Wformat-signedness \
             -fsized-deallocation \
             -Wsized-deallocation \
             -Wsuggest-override \
             -Warray-bounds=2"
        )
    endif ()

    # Enabling some other warnings available since GCC 6
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Wduplicated-cond \
             -Wnull-dereference"
        )
    endif ()

    # Enabling some other warnings available since GCC 7
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 7 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Walloc-zero -Walloca \
             -Wformat-overflow \
             -Wshadow=compatible-local \
             -Waligned-new"
        )
    endif ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(
        CLANG_FLAGS

        "-pedantic -pedantic-errors \
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
         -Wno-documentation -Wno-documentation-unknown-command"
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
elseif (MSVC)
    set(
        MSVC_FLAGS

        "/W4 \
         \
         /wd4100 \
         /wd4127 \
         /wd4131 \
         /wd4152 \
         /wd4204 \
         /wd4244 \
         /wd4245 \
         /wd4267 \
         /wd4456 \
         /wd4996" # Disabling warnings triggered by libpng, GLEW & GLFW
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_FLAGS}")
endif ()

# FBX SDK usage
if (MSVC OR CMAKE_COMPILER_IS_GNUCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is triggered by IS_GNUCC
    option(RAZ_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" ON)

    if (RAZ_USE_FBX)
        if ("${CMAKE_BUILD_TYPE}" MATCHES "(D|d)eb")
            set(BUILD_TYPE "debug")
        elseif ("${CMAKE_BUILD_TYPE}" MATCHES "(R|r)el")
            set(BUILD_TYPE "release")
        endif ()

        if (MSVC)
            # Wildcards used to fetch from 'Program Files (x86)' as well, plus for every version: 2019.0, 2018.1.1, 2018.0, ...
            # TODO: minor, but should be sorted in reverse order: the more recent the better (always possible to manually select it)
            set(FBX_PATH "Program Files*/Autodesk/FBX/FBX SDK/*")
            file(
                GLOB
                SEARCH_PATHS

                "C:/${FBX_PATH}"
                "D:/${FBX_PATH}"
                "E:/${FBX_PATH}"
            )

            # TODO: searching by default into 'vs2015'; should be MSVC-version dependent
            set(FBX_LIB_DIR "lib/vs2015/x64/${BUILD_TYPE}")
            set(FBX_LIB_SUBPATH "${FBX_LIB_DIR}/libfbxsdk.lib")
        elseif (CMAKE_COMPILER_IS_GNUCC)
            file(
                GLOB
                SEARCH_PATHS

                "/usr"
                "/usr/local"
                "~/*FBX*"
                "~/*fbx*"
             )

            set(FBX_LIB_DIR "lib/gcc4/x64/${BUILD_TYPE}")
            set(FBX_LIB_SUBPATH "${FBX_LIB_DIR}/libfbxsdk.a")
        endif ()

        # Checking if correctly user-assigned or previously found
        if (EXISTS ${FBX_ROOT_DIR}/include/fbxsdk.h AND EXISTS ${FBX_ROOT_DIR}/${FBX_LIB_SUBPATH})
            set(FBX_FOUND TRUE)
            set(LIBS ${LIBS} ${FBX_ROOT_DIR}/${FBX_LIB_SUBPATH})
        else () # Not existing at requested place, searching for it elsewhere
            foreach (PATH ${SEARCH_PATHS})
                set(FBX_LIB "${PATH}/${FBX_LIB_SUBPATH}")

                # If header & lib exist, include & link each other respectively
                if (EXISTS "${PATH}/include/fbxsdk.h" AND EXISTS ${FBX_LIB})
                    set(FBX_ROOT_DIR ${PATH} CACHE PATH "Path to your FBX SDK root directory" FORCE)
                    set(FBX_FOUND TRUE)
                    set(LIBS ${LIBS} ${FBX_LIB})

                    break()
                endif ()
            endforeach ()
        endif ()

        if (FBX_FOUND)
            add_definitions(-DFBX_ENABLED)
            add_definitions(-DFBXSDK_SHARED)
            include_directories(${FBX_ROOT_DIR}/include)

            message("FBX SDK linked.")
        else ()
            set(FBX_ROOT_DIR "" CACHE PATH "Path to your FBX SDK root directory")
            message(SEND_ERROR "Error: Couldn't find FBX SDK in expected places, please set its root directory manually.")
        endif ()
    endif ()
endif ()

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

    src/RaZ/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/*.hpp
    include/RaZ/*.inl
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
            LIBS

            ${LIBS}
            asan
            ubsan
        )
    endif ()

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

    # To automatically export all the classes & functions
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)
endif ()

# If FBX SDK not found, do not compile FBX loader
if (NOT FBX_FOUND)
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
    SOURCE_FILES

    ${SRC}
)

# Removing unwanted platform-specific files
list(
    REMOVE_ITEM
    SOURCE_FILES

    ${DISPOSABLE}
)

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC ${SOURCE_FILES})
else ()
    add_library(RaZ SHARED ${SOURCE_FILES})
endif ()

target_link_libraries(
    RaZ

    ${LIBS}
    ${DEPS}
)

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
