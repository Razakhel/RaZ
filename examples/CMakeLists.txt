project(RaZ_Examples)

# Defining an interface library to hold common settings for all examples
add_library(RaZ_Examples INTERFACE)
target_compile_features(RaZ_Examples INTERFACE cxx_std_17)
target_link_libraries(RaZ_Examples INTERFACE RaZ)
target_sources(RaZ_Examples INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/DemoUtils.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/DemoUtils.hpp")

include(CompilerFlags)
add_compiler_flags(RaZ_Examples INTERFACE)

# Disabling some warnings, which are intendedly present
if (RAZ_COMPILER_GCC)
    set(
        RAZ_EXAMPLES_DISABLED_WARNINGS

        -Wno-unused-variable
    )
elseif (RAZ_COMPILER_CLANG)
    set(
        RAZ_EXAMPLES_DISABLED_WARNINGS

        -Wno-header-hygiene
        -Wno-unused-variable
    )
elseif (RAZ_COMPILER_MSVC)
    set(
        RAZ_EXAMPLES_DISABLED_WARNINGS

        # Warnings triggered by MSVC's standard library
        /wd4189 # Local variable is initalized but not referenced (unused variable)
        /wd5220 # Non-static volatile member doesn't imply non-trivial move/copy ctor/operator=
    )
endif ()

target_compile_options(RaZ_Examples INTERFACE ${RAZ_EXAMPLES_DISABLED_WARNINGS})

if (RAZ_USE_AUDIO)
    add_executable(RaZ_AudioDemo audioDemo.cpp)
    target_link_libraries(RaZ_AudioDemo RaZ_Examples)
endif ()

add_executable(RaZ_BloomDemo bloomDemo.cpp)
target_link_libraries(RaZ_BloomDemo RaZ_Examples)

if (NOT RAZ_USE_EMSCRIPTEN)
    add_executable(RaZ_ComputeDemo computeDemo.cpp)
    target_link_libraries(RaZ_ComputeDemo RaZ_Examples)
endif ()

add_executable(RaZ_DeferredDemo deferredDemo.cpp)
target_link_libraries(RaZ_DeferredDemo RaZ_Examples)

add_executable(RaZ_FullDemo fullDemo.cpp)
target_link_libraries(RaZ_FullDemo RaZ_Examples)

add_executable(RaZ_MinDemo minDemo.cpp)
target_link_libraries(RaZ_MinDemo RaZ_Examples)

add_executable(RaZ_PhysicsDemo physicsDemo.cpp)
target_link_libraries(RaZ_PhysicsDemo RaZ_Examples)

if (RAZ_USE_LUA)
    add_executable(RaZ_ScriptDemo scriptDemo.cpp)
    target_link_libraries(RaZ_ScriptDemo RaZ_Examples)
endif()

if (NOT RAZ_USE_EMSCRIPTEN)
    add_executable(RaZ_ShowcaseDemo showcaseDemo.cpp)
    target_link_libraries(RaZ_ShowcaseDemo RaZ_Examples)
endif ()

add_executable(RaZ_SSRDemo ssrDemo.cpp)
target_link_libraries(RaZ_SSRDemo RaZ_Examples)

if (NOT RAZ_USE_EMSCRIPTEN)
    add_executable(RaZ_TessellationDemo tessellationDemo.cpp)
    target_link_libraries(RaZ_TessellationDemo RaZ_Examples)
endif ()

if (RAZ_USE_EMSCRIPTEN)
    set_target_properties(RaZ_BloomDemo    PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_DeferredDemo PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_FullDemo     PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_MinDemo      PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_PhysicsDemo  PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_SSRDemo      PROPERTIES SUFFIX ".html")

    if (RAZ_USE_AUDIO)
        set_target_properties(RaZ_AudioDemo PROPERTIES SUFFIX ".html")
    endif ()

    if (RAZ_USE_LUA)
        set_target_properties(RaZ_ScriptDemo PROPERTIES SUFFIX ".html")
    endif()

    include(EmscriptenAssets.cmake)
endif ()

# Copying DLLs to the examples folder to link the libraries properly
if (WIN32)
    if (RAZ_USE_AUDIO)
        file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/examples/OpenAL32.dll")
        file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/examples/libopenal-1.dll")
    endif ()

    if (MSVC AND RAZ_USE_FBX)
        file(COPY "${FBX_DLL}" DESTINATION "${CMAKE_BINARY_DIR}/examples/")
    endif ()
endif ()
