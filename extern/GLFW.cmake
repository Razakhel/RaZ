########
# GLFW #
########

project(GLFW)

add_library(GLFW OBJECT)

target_compile_features(GLFW PRIVATE c_std_11)

# Defining preprocessor macros and selecting files to be removed
if (RAZ_PLATFORM_WINDOWS OR RAZ_PLATFORM_CYGWIN)
    set(
        GLFW_DEFINITIONS

        -D_GLFW_WIN32
    )

    set(
        GLFW_PLATFORM_SRC

        glfw/src/egl*
        glfw/src/osmesa*
        glfw/src/wgl*
        glfw/src/win32*
    )

    if (RAZ_PLATFORM_CYGWIN)
        # GLFW needs to be linked against the Windows Gdi32 library with Cygwin
        set(GLFW_LINKER_FLAGS Gdi32)
    endif ()
elseif (RAZ_PLATFORM_MAC)
    set(
        GLFW_DEFINITIONS

        -D_GLFW_COCOA
        -D_GLFW_USE_CHDIR
        -D_GLFW_USE_MENUBAR
    )

    set(
        GLFW_PLATFORM_SRC

        glfw/src/cocoa*
        glfw/src/egl*
        glfw/src/nsgl*
        glfw/src/osmesa*
        glfw/src/posix_thread*
    )

    set(
        GLFW_LINKER_FLAGS

        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif (RAZ_PLATFORM_LINUX)
    option(GLFW_USE_WAYLAND "Use Wayland instead of X11" OFF)

    if (GLFW_USE_WAYLAND)
        # Using Wayland
        set(
            GLFW_DEFINITIONS

            -D_GLFW_WAYLAND
        )

        set(
            GLFW_PLATFORM_SRC

            glfw/src/wl*
        )
    else ()
        # Using X11
        set(
            GLFW_DEFINITIONS

            -D_GLFW_X11
        )

        set(
            GLFW_PLATFORM_SRC

            glfw/src/glx*
            glfw/src/x11*
        )
    endif ()


    set(
        GLFW_PLATFORM_SRC

        ${GLFW_PLATFORM_SRC}
        glfw/src/egl*
        glfw/src/linux*
        glfw/src/osmesa*
        glfw/src/posix*
        glfw/src/xkb*
    )

    set(
        GLFW_LINKER_FLAGS

        dl
        GL
        pthread
        X11
        Xcursor
        Xinerama
        Xrandr
        Xxf86vm
    )
endif ()

file(
    GLOB
    GLFW_FILES

    # Common files
    glfw/src/context.c
    glfw/src/init.c
    glfw/src/input.c
    glfw/src/monitor.c
    glfw/src/vulkan.c
    glfw/src/window.c

    # Platform-specific files
    ${GLFW_PLATFORM_SRC}
)

target_sources(GLFW PRIVATE ${GLFW_FILES})

target_include_directories(
    GLFW
    SYSTEM

    PUBLIC
        glfw/include
        glfw/deps
)

target_compile_definitions(GLFW PUBLIC ${GLFW_DEFINITIONS})

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(GLFW PRIVATE /w)
else ()
    target_compile_options(GLFW PRIVATE -w)
endif ()

target_link_libraries(GLFW PUBLIC ${GLFW_LINKER_FLAGS})
