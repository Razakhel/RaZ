######################
# RaZ - Dependencies #
######################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT RAZ_USE_EMSCRIPTEN)
    option(RAZ_USE_GLEW "Use GLEW" ON)
    if (RAZ_USE_GLEW)
        include(GLEW)
    endif ()

    option(RAZ_USE_WINDOW "Use windowing & input capabilities (with GLFW)" ON)
    if (RAZ_USE_WINDOW)
        include(GLFW)
    endif ()

    option(RAZ_USE_NETWORKING "Use networking capabilities (with Asio)" ON)
    if (RAZ_USE_NETWORKING)
        # Asio requires threading capabilities which aren't yet enabled with Emscripten
        include(Asio)
    endif ()

    if (NOT RAZ_PLATFORM_MAC AND RAZ_USE_WINDOW)
        # XR currently isn't available with macOS or Emscripten and requires windowing capabilities
        include(OpenXR)
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
