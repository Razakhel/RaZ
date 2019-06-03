###################
# Finding FBX SDK #
###################

if ("${CMAKE_BUILD_TYPE}" MATCHES "(D|d)eb")
    set(BUILD_TYPE "debug")
else ()
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
    set(FBX_LIB ${FBX_ROOT_DIR}/${FBX_LIB_SUBPATH})
else () # Not existing at requested place, searching for it elsewhere
    foreach (PATH ${SEARCH_PATHS})
        set(FBX_LIB_LOCATION "${PATH}/${FBX_LIB_SUBPATH}")

        # If header & lib exist, include & link each other respectively
        if (EXISTS "${PATH}/include/fbxsdk.h" AND EXISTS ${FBX_LIB_LOCATION})
            set(FBX_ROOT_DIR ${PATH} CACHE PATH "Path to your FBX SDK root directory" FORCE)
            set(FBX_FOUND TRUE)
            set(FBX_LIB ${FBX_LIB} ${FBX_LIB_LOCATION})

            break()
        endif ()
    endforeach ()
endif ()

if (FBX_FOUND)
    add_definitions(-DFBX_ENABLED)
    add_definitions(-DFBXSDK_SHARED)
    include_directories(${FBX_ROOT_DIR}/include)
else ()
    set(FBX_ROOT_DIR "" CACHE PATH "Path to your FBX SDK root directory")
    message(SEND_ERROR "Error: Couldn't find FBX SDK in expected places, please set its root directory manually.")
endif ()
