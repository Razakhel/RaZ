###################
# Finding FBX SDK #
###################

set(FBX_ROOT_DIR "" CACHE PATH "Path to your FBX SDK root directory")

# If a directory is manually specified, use it to search for the needed files; otherwise, an attempt to find it is made
if (FBX_ROOT_DIR)
    set(FBX_SEARCH_PATHS "${FBX_ROOT_DIR}")
else ()
    if (WIN32)
        # Wildcards used to search in 'Program Files (x86)' as well, and to find every version (20XX.X.X)
        set(FBX_PATH "Program Files*/Autodesk/FBX/FBX SDK/*")
        file(
            GLOB
            FBX_SEARCH_PATHS

            "C:/${FBX_PATH}"
            "D:/${FBX_PATH}"
            "E:/${FBX_PATH}"
        )

        list(SORT FBX_SEARCH_PATHS ORDER DESCENDING) # Sorting by descending order, to supposedly have the latest version first
    elseif (UNIX AND NOT APPLE)
        file(
            GLOB
            FBX_SEARCH_PATHS

            "/home/*/*FBX*"
            "/home/*/*fbx*"
            "/usr/local"
            "/usr"
        )
    endif ()
endif ()

find_path(
    FBX_INCLUDE_DIR

    NAMES
        fbxsdk.h
    HINTS
        ENV FBX_ROOT
    PATHS
        ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES
        include
)

if (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES "(D|d)eb")
    set(FBX_CONFIG "debug")
else ()
    set(FBX_CONFIG "release")
endif ()

find_library(
    FBX_LIBRARY

    NAMES
        libfbxsdk
        libfbxsdk.a # CMake does not seem to find libraries with an extension under Linux (and presumably macOS); it must be specified
    HINTS
        ENV FBX_ROOT
    PATHS
        ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES
        lib/vs2022/x64/${FBX_CONFIG}
        lib/vs2019/x64/${FBX_CONFIG}
        lib/vs2017/x64/${FBX_CONFIG}
        lib/vs2015/x64/${FBX_CONFIG}

        lib/gcc/x64/${FBX_CONFIG}
        lib/gcc4/x64/${FBX_CONFIG}
)

if (WIN32)
    # Under Windows, finding the DLL may be useful in some cases
    find_file(
        FBX_DLL

        NAMES
            libfbxsdk.dll
        HINTS
            ENV FBX_ROOT
        PATHS
            ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES
            lib/vs2022/x64/${FBX_CONFIG}
            lib/vs2019/x64/${FBX_CONFIG}
            lib/vs2017/x64/${FBX_CONFIG}
            lib/vs2015/x64/${FBX_CONFIG}
    )
endif ()

if (FBX_LIBRARY AND FBX_INCLUDE_DIR)
    set(FBX_FOUND ON)
    set(FBX_DEFINITIONS -DFBXSDK_SHARED)

    message(STATUS "[FBX] Found:")
    message(STATUS "  - Include directory: ${FBX_INCLUDE_DIR}")
    message(STATUS "  - Library: ${FBX_LIBRARY}")
    if (WIN32)
        message(STATUS "  - DLL: ${FBX_DLL}")
    endif ()

    if (UNIX)
        list(APPEND FBX_LIBRARY xml2) # The SDK requires libxml2
    endif ()
endif ()
