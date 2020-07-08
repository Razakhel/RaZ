######################
# Finding OpenAL SDK #
######################

if (WIN32 OR CYGWIN)
    # If a directory is manually specified, use it to search for the needed files; otherwise, an attempt to find it is made
    if (OPENAL_SDK_ROOT)
        set(OPENAL_SEARCH_PATHS ${OPENAL_SDK_ROOT})
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
        OPENAL_LIBS

        NAMES
            OpenAL al openal OpenAL32
        HINTS
            ENV OPENALDIR
            ENV OPENAL_SDK_PATH
        PATH_SUFFIXES
            lib64 lib libs64 libs libs/Win64
        PATHS
            ${OPENAL_SEARCH_PATHS}
        REQUIRED
        NO_DEFAULT_PATH
    )
elseif (APPLE)
    set(OPENAL_LIBS "-framework OpenAL")

    file(
        GLOB
        OPENAL_SEARCH_PATHS

        "/System/Library/Frameworks/openal*"
        "~/Library/Frameworks/openal*"
        "/Library/Frameworks/openal*"
        "/opt/openal*"
        "/usr/local/opt/openal*"
    )
else () # Linux
    set(OPENAL_LIBS openal)

    file(
        GLOB
        OPENAL_SEARCH_PATHS

        "/usr/include"
        "/usr/local/include"
        "~/OpenAL*"
    )
endif ()

find_path(
    OPENAL_INCLUDE_DIRS

    NAMES
        AL/al.h
        AL/alc.h
    HINTS
        ENV OPENALDIR
        ENV OPENAL_SDK_PATH
    PATH_SUFFIXES
        include/AL include/OpenAL include AL OpenAL
    PATHS
        ${OPENAL_SEARCH_PATHS}
    REQUIRED
)

if (OPENAL_INCLUDE_DIRS AND OPENAL_LIBS)
    set(OpenAL_FOUND ON)
    message("[OpenAL] Found:")
else ()
    message("[OpenAL] NOT found:")
endif ()

message("  - Include directory: ${OPENAL_INCLUDE_DIRS}")
message("  - Library: ${OPENAL_LIBS}")
