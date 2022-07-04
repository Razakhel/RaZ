###################
# Finding FBX SDK #
###################

set(FBX_ROOT_DIR "" CACHE PATH "Path to your FBX SDK root directory")

# If the FBX_ROOT_DIR variable has not been given, attempting to find the SDK's location
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
    FBX_INCLUDE_DIRS

    NAMES
        fbxsdk.h
    HINTS
        ENV FBX_ROOT
    PATHS
        ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES
        include
    REQUIRED
)

message("[FBX] Found include directory: ${FBX_INCLUDE_DIRS}")

if (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES "(D|d)eb")
    set(FBX_CONFIG "debug")
else ()
    set(FBX_CONFIG "release")
endif ()

find_library(
    FBX_LIBS

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
    REQUIRED
)

message("[FBX] Found library: ${FBX_LIBS}")

if (UNIX)
    list(APPEND FBX_LIBS xml2) # The SDK requires libxml2
endif ()

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
        REQUIRED
    )

    message("[FBX] Found DLL: ${FBX_DLL}")
endif ()

set(FBX_FOUND ON)
set(
    FBX_DEFINITIONS

    -DFBX_ENABLED
    -DFBXSDK_SHARED
)
