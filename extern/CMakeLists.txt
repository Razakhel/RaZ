######################
# RaZ - Dependencies #
######################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

option(RAZ_USE_LIBPNG "Compile & use libpng" ON)
if (RAZ_USE_LIBPNG)
    include(libpng)
endif ()

option(RAZ_USE_GLEW "Compile & use GLEW" ON)
if (RAZ_USE_GLEW)
    include(GLEW)
endif ()

option(RAZ_USE_GLFW "Compile & use GLFW" ON)
if (RAZ_USE_GLFW)
    include(GLFW)
endif ()

option(RAZ_USE_IMGUI "Compile & use ImGui" ON)
if (RAZ_USE_IMGUI)
    include(ImGui)
endif ()
