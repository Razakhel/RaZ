project(RaZ_Dependencies)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(png PNG_SRC)
aux_source_directory(glew/src GLEW_SRC)
aux_source_directory(glfw/src GLFW_SRC)
aux_source_directory(imgui IMGUI_SRC)

set(
    RAZ_DEPS_SRC

    ${PNG_SRC}
    ${GLEW_SRC}
    ${GLFW_SRC}
    ${IMGUI_SRC}
)

# Defining preprocessor macros and selecting files to be removed
if (APPLE)
    set(
        RAZ_DEPS_DEFINITIONS

        -D_GLFW_COCOA
        -D_GLFW_USE_CHDIR
        -D_GLFW_USE_MENUBAR
        -D_GLFW_USE_RETINA
    )

    file(
        GLOB
        RAZ_DEPS_UNUSED_FILES

        glfw/src/win32*
        glfw/src/x11*
        glfw/src/wl*
        glfw/src/mir*
        glfw/src/glx*
        glfw/src/linux*
    )
elseif (UNIX)
    set(
        RAZ_DEPS_DEFINITIONS

        -D_GLFW_X11
        -D_GLFW_HAS_XF86VM
    )

    file(
        GLOB
        RAZ_DEPS_UNUSED_FILES

        glfw/src/cocoa*
        glfw/src/win32*
        glfw/src/wl*
        glfw/src/wgl*
        glfw/src/mir*
        glfw/src/*.m
    )

    set(
        RAZ_DEPS_LINKER_FLAGS

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
    set(
        RAZ_DEPS_DEFINITIONS

        -D_GLFW_WIN32
        -DGLEW_BUILD
        #-DGLEW_NO_GLU
    )

    file(
        GLOB
        RAZ_DEPS_UNUSED_FILES

        glfw/src/cocoa*
        glfw/src/x11*
        glfw/src/wl*
        glfw/src/mir*
        glfw/src/glx*
        glfw/src/linux*
        glfw/src/posix*
        glfw/src/*.m
    )

    set(
        RAZ_DEPS_LINKER_FLAGS

        opengl32
    )
endif ()

# Adding recursively every file we want to compile
file(
    GLOB
    RAZ_DEPS_FILES

    ${RAZ_DEPS_SRC}
)

# Removing unwanted platform-specific files
list(
    REMOVE_ITEM
    RAZ_DEPS_FILES

    ${RAZ_DEPS_UNUSED_FILES}
)

# Building externals
if (RAZ_BUILD_STATIC)
    add_library(RaZ_Dependencies STATIC ${RAZ_DEPS_FILES})
else ()
    add_library(RaZ_Dependencies SHARED ${RAZ_DEPS_FILES})
endif ()

target_include_directories(
    RaZ_Dependencies

    SYSTEM # The dependencies' headers are included as system ones, disabling all warnings
    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/png
    ${CMAKE_CURRENT_SOURCE_DIR}/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

target_compile_definitions(RaZ_Dependencies PUBLIC ${RAZ_DEPS_DEFINITIONS})

# Disabling all compilers warnings for external dependencies
if (MSVC)
    set(RAZ_DEPS_COMPILER_FLAGS /w)
else ()
    set(RAZ_DEPS_COMPILER_FLAGS -w)
endif ()

target_compile_options(RaZ_Dependencies PRIVATE ${RAZ_DEPS_COMPILER_FLAGS})
target_link_libraries(RaZ_Dependencies PRIVATE ${RAZ_DEPS_LINKER_FLAGS})
