project(RaZ_Examples)

# Defining an interface library to hold common settings for all examples
add_library(RaZ_Examples INTERFACE)
target_compile_features(RaZ_Examples INTERFACE cxx_std_17)
target_link_libraries(RaZ_Examples INTERFACE RaZ)
target_sources(RaZ_Examples INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/DemoUtils.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/DemoUtils.hpp")

include(CompilerFlags)
add_compiler_flags(TARGET RaZ_Examples SCOPE INTERFACE ${SANITIZERS_OPTION})

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

macro (add_example TARGET_NAME MAIN_FILE)
    add_executable(${TARGET_NAME} ${MAIN_FILE})
    target_link_libraries(${TARGET_NAME} RaZ_Examples)
    list(APPEND EXAMPLE_TARGETS ${TARGET_NAME})
endmacro ()

if (RAZ_USE_WINDOW OR RAZ_USE_EMSCRIPTEN)
    add_example(RaZ_BloomDemo    bloomDemo.cpp)
    add_example(RaZ_DeferredDemo deferredDemo.cpp)
    add_example(RaZ_FullDemo     fullDemo.cpp)
    add_example(RaZ_MinDemo      minDemo.cpp)
    add_example(RaZ_PhysicsDemo  physicsDemo.cpp)
    add_example(RaZ_SSRDemo      ssrDemo.cpp)

    if (RAZ_USE_AUDIO)
        add_example(RaZ_AudioDemo audioDemo.cpp)
    endif ()

    if (RAZ_USE_LUA)
        add_example(RaZ_ScriptDemo scriptDemo.cpp)
    endif ()

    if (NOT RAZ_USE_EMSCRIPTEN)
        add_example(RaZ_ComputeDemo      computeDemo.cpp)      # Compute shaders are unavailable with WebGL
        add_example(RaZ_ShowcaseDemo     showcaseDemo.cpp)     # Too many assets to be built (command line too long)
        add_example(RaZ_TessellationDemo tessellationDemo.cpp) # Tessellation shaders are unavailable with WebGL

        if (NOT RAZ_PLATFORM_MAC)
            add_example(RaZ_XRDemo xrDemo.cpp) # XR currently isn't available with macOS or Emscripten
        endif ()
    endif ()
endif ()

foreach (EXAMPLE_TARGET IN LISTS EXAMPLE_TARGETS)
    if (RAZ_USE_EMSCRIPTEN)
        set_target_properties(${EXAMPLE_TARGET} PROPERTIES SUFFIX ".html")
    endif ()

    if (WIN32 AND RAZ_USE_FBX)
        add_custom_command(
            TARGET ${EXAMPLE_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${EXAMPLE_TARGET}>
                $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
            COMMAND_EXPAND_LISTS
        )
    endif ()
endforeach ()

if (RAZ_USE_EMSCRIPTEN)
    include(EmscriptenAssets.cmake)
endif ()

# Copying DLLs to the examples folder to link the libraries properly
if (WIN32 AND RAZ_USE_AUDIO)
    file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/examples/OpenAL32.dll")
    file(COPY_FILE "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}/examples/libopenal-1.dll")
endif ()
