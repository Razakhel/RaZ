#########
# ImGui #
#########

project(ImGui)

# If not compiling with Emscripten, GLEW and GLFW need to be enabled for ImGui to be available
if (NOT RAZ_USE_EMSCRIPTEN)
    if (NOT RAZ_USE_GLEW)
        message(FATAL_ERROR "[RaZ] ImGui can't be used with GLEW disabled; either reenable the latter or disable the former")
    elseif (NOT RAZ_USE_GLFW)
        message(FATAL_ERROR "[RaZ] ImGui can't be used with GLFW disabled; either reenable the latter or disable the former")
    endif ()
endif ()

add_library(ImGui OBJECT)

target_compile_features(ImGui PRIVATE cxx_std_17)

# The needed Freetype header ft2build.h seems to be found only when calling aux_source_directory(); this call is thus kept instead of manually finding files
aux_source_directory(imgui IMGUI_SRC)

set(
    IMGUI_SRC

    ${IMGUI_SRC}
    imgui/*.h
    imgui/misc/cpp/*.cpp
)

file(
    GLOB_RECURSE
    IMGUI_FILES

    ${IMGUI_SRC}
)

if (NOT RAZ_USE_GLFW AND NOT RAZ_USE_EMSCRIPTEN)
    list(
        REMOVE_ITEM
        IMGUI_FILES

        imgui/imgui_impl_glfw.h
        imgui/imgui_impl_glfw.cpp
     )
endif ()

# Building ImGui
target_sources(ImGui PRIVATE ${IMGUI_FILES})

target_include_directories(
    ImGui

    SYSTEM
    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/imgui # Needed for imgui_stdlib.cpp
    glew/include
)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(ImGui PRIVATE /w)
else ()
    target_compile_options(ImGui PRIVATE -w)

    if (RAZ_USE_EMSCRIPTEN)
        target_compile_options(ImGui PRIVATE -DIMGUI_IMPL_OPENGL_ES3 -s USE_GLFW=3)
    endif ()
endif ()

target_link_libraries(ImGui PUBLIC GLEW)

if (RAZ_USE_GLFW)
    target_link_libraries(ImGui PUBLIC GLFW)
endif ()
