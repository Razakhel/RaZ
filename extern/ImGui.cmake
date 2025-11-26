#########
# ImGui #
#########

project(ImGui)

# If not compiling with Emscripten, GLEW and GLFW need to be enabled for ImGui to be available
if (NOT RAZ_USE_EMSCRIPTEN)
    if (NOT RAZ_USE_GLEW OR NOT RAZ_USE_WINDOW)
        message(FATAL_ERROR "[RaZ] ImGui can't be used with either GLEW or windowing disabled; either reenable both or disable ImGui")
    endif ()
endif ()

add_library(ImGui OBJECT)

target_compile_features(ImGui PRIVATE cxx_std_17)

file(
    GLOB
    IMGUI_FILES

    imgui/misc/cpp/*.cpp
    imgui/misc/cpp/*.h
    imgui/*.cpp
    imgui/*.h
    implot/*.cpp
    implot/*.h
)

if (NOT RAZ_USE_WINDOW AND NOT RAZ_USE_EMSCRIPTEN)
    list(
        REMOVE_ITEM
        IMGUI_FILES

        imgui/imgui_impl_glfw.h
        imgui/imgui_impl_glfw.cpp
     )
endif ()

target_sources(ImGui PRIVATE ${IMGUI_FILES})

target_include_directories(
    ImGui
    SYSTEM

    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/implot
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

if (RAZ_USE_WINDOW AND NOT RAZ_USE_EMSCRIPTEN)
    target_link_libraries(ImGui PUBLIC GLFW)
endif ()
