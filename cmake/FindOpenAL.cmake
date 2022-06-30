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
        PATHS
            ${OPENAL_SEARCH_PATHS}
        PATH_SUFFIXES
            lib64 lib libs64 libs libs/Win64
        REQUIRED
    )

    if (WIN32)
        # Under Windows, finding the DLL may be useful in some cases
        find_file(
            OPENAL_DLL

            NAMES
                soft_oal.dll
            HINTS
                ENV OPENALDIR
                ENV OPENAL_SDK_PATH
            PATHS
                ${OPENAL_SEARCH_PATHS}
            PATH_SUFFIXES
                bin bin/Win64
        )
    endif ()
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

        "/home/*/OpenAL*"
        "/home/*/openal*"
        "/usr/local/include"
        "/usr/include"
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
    PATHS
        ${OPENAL_SEARCH_PATHS}
    PATH_SUFFIXES
        include/AL include/OpenAL include AL OpenAL
    REQUIRED
)

set(OpenAL_FOUND ON)

message("[OpenAL] Found:")
message("  - Include directory: ${OPENAL_INCLUDE_DIRS}")
message("  - Library: ${OPENAL_LIBS}")
if (WIN32)
    message("  - DLL: ${OPENAL_DLL}")
endif ()
