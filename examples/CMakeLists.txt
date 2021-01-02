project(RaZ_Examples)

# Defining an interface library to hold common settings for all examples
add_library(RaZ_Examples INTERFACE)
target_compile_features(RaZ_Examples INTERFACE cxx_std_17)
target_link_libraries(RaZ_Examples INTERFACE RaZ)

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

add_executable(RaZ_DeferredDemo deferredDemo.cpp)
target_link_libraries(RaZ_DeferredDemo RaZ_Examples)

add_executable(RaZ_FullDemo fullDemo.cpp)
target_link_libraries(RaZ_FullDemo RaZ_Examples)

add_executable(RaZ_MinDemo minDemo.cpp)
target_link_libraries(RaZ_MinDemo RaZ_Examples)

add_executable(RaZ_ShowcaseDemo showcaseDemo.cpp)
target_link_libraries(RaZ_ShowcaseDemo RaZ_Examples)

add_executable(RaZ_SoundDemo soundDemo.cpp)
target_link_libraries(RaZ_SoundDemo RaZ_Examples)

if (RAZ_USE_EMSCRIPTEN)
    set_target_properties(RaZ_DeferredDemo PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_FullDemo     PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_MinDemo      PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_ShowcaseDemo PROPERTIES SUFFIX ".html")
    set_target_properties(RaZ_SoundDemo    PROPERTIES SUFFIX ".html")

    include(EmscriptenAssets.cmake)
endif ()

# If FBX SDK used with MSVC, copy the DLL into the examples/ folder to link it properly
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    set(DLL_COPY_COMMAND ${CMAKE_COMMAND} -E copy "${FBX_CONFIG_DLL}" "${CMAKE_BINARY_DIR}/examples/${FBX_LIB_NAME}.dll")

    add_custom_command(TARGET RaZ_DeferredDemo PRE_LINK COMMAND ${DLL_COPY_COMMAND})
    add_custom_command(TARGET RaZ_FullDemo     PRE_LINK COMMAND ${DLL_COPY_COMMAND})
    add_custom_command(TARGET RaZ_MinDemo      PRE_LINK COMMAND ${DLL_COPY_COMMAND})
    add_custom_command(TARGET RaZ_ShowcaseDemo PRE_LINK COMMAND ${DLL_COPY_COMMAND})
endif ()
