###################
# Finding FBX SDK #
###################

if (MSVC)
    # TODO: searching by default into 'vs2015'; should be MSVC-version dependent
    set(FBX_LIB_DIR "${FBX_ROOT_DIR}/lib/vs2015/x64")

    set(FBX_LIB_NAME "libfbxsdk")
    set(FBX_DEBUG_LIB "${FBX_LIB_DIR}/debug/${FBX_LIB_NAME}")
    set(FBX_RELEASE_LIB "${FBX_LIB_DIR}/release/${FBX_LIB_NAME}")

    # Checking that .lib & .dll exist, either in debug or release
    if ((EXISTS "${FBX_DEBUG_LIB}.lib" AND EXISTS "${FBX_DEBUG_LIB}.dll") OR (EXISTS "${FBX_RELEASE_LIB}.lib" AND EXISTS "${FBX_RELEASE_LIB}.dll"))
        # The $(Configuration) macro is later replaced by VS automatically according to the current configuration (Debug, Release, etc)
        set(FBX_LIB "${FBX_LIB_DIR}/$(Configuration)/${FBX_LIB_NAME}.lib")

        set(FBX_MSVC_DLL "${FBX_LIB_DIR}/$(Configuration)/${FBX_LIB_NAME}.dll")
        set(FBX_CONFIG_DLL "${FBX_LIB_DIR}/$<CONFIG>/${FBX_LIB_NAME}.dll")

        set(FBX_FOUND TRUE)
    endif ()
elseif (CMAKE_COMPILER_IS_GNUCC)
    if ("${CMAKE_BUILD_TYPE}" MATCHES "(D|d)eb")
        set(BUILD_TYPE "debug")
    else ()
        set(BUILD_TYPE "release")
    endif ()

    set(FBX_LIB "${FBX_ROOT_DIR}/lib/gcc4/x64/${BUILD_TYPE}/libfbxsdk.a")

    # Checking if correctly user-assigned or previously found
    if (EXISTS "${FBX_ROOT_DIR}/include/fbxsdk.h" AND EXISTS "${FBX_LIB}")
        set(FBX_FOUND TRUE)
    endif ()
endif ()

# SDK not found yet at the requested place, searching for it elsewhere
if (NOT FBX_FOUND)
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
    elseif (CMAKE_COMPILER_IS_GNUC)
        file(
            GLOB
            SEARCH_PATHS

            "/usr"
            "/usr/local"
            "~/*FBX*"
            "~/*fbx*"
        )
    endif ()

    foreach (PATH ${SEARCH_PATHS})
        set(FBX_LIB_LOCATION "${PATH}/${FBX_LIB_DIR}")

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
