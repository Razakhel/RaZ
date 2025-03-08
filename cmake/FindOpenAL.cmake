##################
# Finding OpenAL #
##################

if (WIN32 OR CYGWIN)
    set(OPENAL_ROOT_DIR "" CACHE PATH "Path to your OpenAL root directory")

    # If a directory is manually specified, use it to search for the needed files; otherwise, an attempt to find it is made
    if (OPENAL_ROOT_DIR)
        set(OPENAL_SEARCH_PATHS ${OPENAL_ROOT_DIR})
    else ()
        if (WIN32)
            file(
                GLOB
                OPENAL_SEARCH_PATHS

                "C:/OpenAL*"
                "D:/OpenAL*"
                "E:/OpenAL*"
                "C:/Lib*/OpenAL*"
                "D:/Lib*/OpenAL*"
                "E:/Lib*/OpenAL*"
            )
        elseif (CYGWIN)
            file(
                GLOB
                OPENAL_SEARCH_PATHS

                "/cygdrive/c/OpenAL*"
                "/cygdrive/d/OpenAL*"
                "/cygdrive/e/OpenAL*"
                "/cygdrive/c/Lib*/OpenAL*"
                "/cygdrive/d/Lib*/OpenAL*"
                "/cygdrive/e/Lib*/OpenAL*"
            )
        endif ()
    endif ()

    find_library(
        OPENAL_LIBRARY

        NAMES
            OpenAL
            al
            openal
            OpenAL32
        HINTS
            ENV OPENALDIR
            ENV OPENAL_SDK_PATH
        PATHS
            ${OPENAL_SEARCH_PATHS}
        PATH_SUFFIXES
            lib64
            lib
            libs64
            libs
            libs/Win64
    )

    if (WIN32)
        find_file(
            OPENAL_DLL

            NAMES
                soft_oal.dll
                libopenal-1.dll
            HINTS
                ENV OPENALDIR
                ENV OPENAL_SDK_PATH
            PATHS
                ${OPENAL_SEARCH_PATHS}
            PATH_SUFFIXES
                bin
                bin/Win64
        )
    endif ()
elseif (APPLE)
    set(OPENAL_LIBRARY "-framework OpenAL")

    file(
        GLOB
        OPENAL_SEARCH_PATHS

        "/System/Library/Frameworks/openal*"
        "~/Library/Frameworks/openal*"
        "/Library/Frameworks/openal*"
        "/opt/openal*"
        "/opt/homebrew/opt/openal*"
        "/usr/local/opt/openal*"
    )
else () # Linux
    set(OPENAL_LIBRARY openal)

    file(
        GLOB
        OPENAL_SEARCH_PATHS

        "/home/*/OpenAL*"
        "/home/*/openal*"
        "/usr/local/include"
        "/usr/include"
    )
endif ()

find_path(
    OPENAL_INCLUDE_DIR

    NAMES
        AL/al.h
        AL/alc.h
    HINTS
        ENV OPENALDIR
        ENV OPENAL_SDK_PATH
    PATHS
        ${OPENAL_SEARCH_PATHS}
    PATH_SUFFIXES
        include/AL
        include/OpenAL
        include
        AL
        OpenAL
)

if (OPENAL_LIBRARY AND OPENAL_INCLUDE_DIR)
    set(OpenAL_FOUND ON)

    message(STATUS "[OpenAL] Found:")
    message(STATUS "  - Include directory: ${OPENAL_INCLUDE_DIR}")
    message(STATUS "  - Library: ${OPENAL_LIBRARY}")
    if (WIN32)
        message(STATUS "  - DLL: ${OPENAL_DLL}")
    endif ()

    if (WIN32)
        add_library(OpenAL SHARED IMPORTED)
        set_target_properties(OpenAL PROPERTIES IMPORTED_IMPLIB ${OPENAL_LIBRARY})

        # The DLL isn't set as the found file doesn't have the expected name; as there doesn't seem to be any easier way, it is copied manually later
        # Executables made through MSYS2 need libopenal-1.dll available, while in other cases they require OpenAL32.dll. The one found is named soft_oal.dll
        # If later it eventually is loaded manually from the code, or there's a way to easily change the name of a required DLL, it should be reenabled
        #if (WIN32)
        #    set_target_properties(OpenAL PROPERTIES IMPORTED_LOCATION ${OPENAL_DLL})
        #endif ()
    else ()
        # Linux & macOS don't reference a file but the system's library; it can't use an imported library target
        add_library(OpenAL INTERFACE)
        target_link_libraries(OpenAL INTERFACE ${OPENAL_LIBRARY})
    endif ()

    target_include_directories(OpenAL SYSTEM INTERFACE ${OPENAL_INCLUDE_DIR})

    add_library(OpenAL::OpenAL ALIAS OpenAL)
endif ()
