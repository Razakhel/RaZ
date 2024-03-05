######################
# RaZ - Dependencies #
######################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT RAZ_USE_EMSCRIPTEN)
    option(RAZ_USE_LIBPNG "Use libpng" ON)
    if (RAZ_USE_LIBPNG)
        include(libpng)
    endif ()

    option(RAZ_USE_GLEW "Use GLEW" ON)
    if (RAZ_USE_GLEW)
        include(GLEW)
    endif ()

    option(RAZ_USE_WINDOW "Use windowing & input capabilities (with GLFW)" ON)
    if (RAZ_USE_WINDOW)
        include(GLFW)
    endif ()
endif ()

option(RAZ_USE_IMGUI "Use ImGui" ON)
if (RAZ_USE_IMGUI)
    include(ImGui)
endif ()

option(RAZ_USE_LUA "Use Lua scripting (with Sol2)" ON)
if (RAZ_USE_LUA)
    include(Sol2)
endif ()

include(Tracy)

option(RAZ_USE_PROFILING "Use profiling capabilities (with Tracy)" OFF)
if (RAZ_USE_PROFILING)
    target_compile_definitions(Tracy PUBLIC TRACY_ENABLE)
endif ()

include(Catch2)
include(fastgltf)
include(stb)
