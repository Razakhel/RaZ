cmake_minimum_required(VERSION 3.11)
project(RaZ)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# If the build type hasn't been specified, defaulting it to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

#################
# RaZ - Library #
#################

option(RAZ_BUILD_STATIC "Build RaZ statically" ON)
if (RAZ_BUILD_STATIC)
    add_library(RaZ STATIC)
else ()
    add_library(RaZ SHARED)
endif ()

# Using C++17
target_compile_features(RaZ PRIVATE cxx_std_17)

##########################
# RaZ - Useful variables #
##########################

# Detect whether Emscripten is being used
if (CMAKE_CXX_COMPILER MATCHES "/em\\+\\+.*$")
    set(RAZ_USE_EMSCRIPTEN ON)
    target_compile_options(RaZ PRIVATE -c)
else ()
    set(RAZ_USE_EMSCRIPTEN OFF)
endif ()

if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Finding exclusively MSVC, not clang-cl
    set(RAZ_COMPILER "MSVC")
    set(RAZ_COMPILER_MSVC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_MSVC)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (MSVC)
        # Using clang-cl, for which both MSVC & Clang are found
        set(RAZ_COMPILER "Clang-cl")
        set(RAZ_COMPILER_CLANG_CL ON)
    else ()
        set(RAZ_COMPILER "Clang")
    endif ()

    set(RAZ_COMPILER_CLANG ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(RAZ_COMPILER "GCC")
    set(RAZ_COMPILER_GCC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_COMPILER_GCC)
endif ()

if (WIN32 OR CYGWIN)
    set(RAZ_PLATFORM "Windows")
    set(RAZ_PLATFORM_WINDOWS ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_WINDOWS)

    if (CYGWIN)
        set(RAZ_PLATFORM "${RAZ_PLATFORM} (Cygwin)")
        set(RAZ_PLATFORM_CYGWIN ON)
        target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_CYGWIN)
    endif ()
elseif (APPLE)
    set(RAZ_PLATFORM "macOS")
    set(RAZ_PLATFORM_MAC ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_MAC)
elseif (RAZ_USE_EMSCRIPTEN)
    set(RAZ_PLATFORM "Emscripten")
    set(RAZ_PLATFORM_EMSCRIPTEN ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_EMSCRIPTEN USE_OPENGL_ES)
elseif (UNIX)
    set(RAZ_PLATFORM "Linux")
    set(RAZ_PLATFORM_LINUX ON)
    target_compile_definitions(RaZ PUBLIC RAZ_PLATFORM_LINUX)
endif ()

if (RAZ_COMPILER_MSVC)
    set(RAZ_CONFIG_DEBUG   "$<IF:$<CONFIG:Debug>,ON,OFF>")
    set(RAZ_CONFIG_RELEASE "$<IF:$<CONFIG:Debug>,OFF,ON>")
    set(RAZ_CONFIG_SHORT   "$<IF:$<CONFIG:Debug>,Debug,Release>")

    target_compile_definitions(RaZ PUBLIC $<IF:$<CONFIG:Debug>,RAZ_CONFIG_DEBUG,RAZ_CONFIG_RELEASE>)

    set(CONFIG_STR_UPPER $<UPPER_CASE:$<CONFIG>>)
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(RAZ_CONFIG_DEBUG   ON)
        set(RAZ_CONFIG_RELEASE OFF)
        set(RAZ_CONFIG_SHORT   "Debug")

        target_compile_definitions(RaZ PUBLIC RAZ_CONFIG_DEBUG)
    else ()
        set(RAZ_CONFIG_DEBUG   OFF)
        set(RAZ_CONFIG_RELEASE ON)
        set(RAZ_CONFIG_SHORT   "Release")

        target_compile_definitions(RaZ PUBLIC RAZ_CONFIG_RELEASE)
    endif ()

    string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_STR_UPPER)
endif ()

set(RAZ_BUILD_FLAGS "${CMAKE_CXX_FLAGS_${CONFIG_STR_UPPER}}")

option(ENABLE_DEBUG_INFO "Creates a debug target which prints useful values" OFF)

if (ENABLE_DEBUG_INFO)
    cmake_host_system_information(RESULT PLATFORM_RELEASE QUERY OS_RELEASE)
    cmake_host_system_information(RESULT PLATFORM_VERSION QUERY OS_VERSION)

    # Trimming potential leading & trailing space characters (happens for Windows' values)
    string(STRIP ${PLATFORM_RELEASE} PLATFORM_RELEASE)
    string(STRIP ${PLATFORM_VERSION} PLATFORM_VERSION)

    add_custom_target(
        RaZ_PrintDebugInfo

        ${CMAKE_COMMAND} -E echo "" &&
        ${CMAKE_COMMAND} -E echo "####################" &&
        ${CMAKE_COMMAND} -E echo "# [RaZ] Debug info #" &&
        ${CMAKE_COMMAND} -E echo "####################" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Platform (found ${RAZ_PLATFORM})" &&
        ${CMAKE_COMMAND} -E echo "  Name:     ${CMAKE_SYSTEM_NAME}" &&
        ${CMAKE_COMMAND} -E echo "  Release:  ${PLATFORM_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${PLATFORM_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Compiler (found ${RAZ_COMPILER})" &&
        ${CMAKE_COMMAND} -E echo "  ID:       ${CMAKE_CXX_COMPILER_ID}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${CMAKE_CXX_COMPILER_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "  Path:     ${CMAKE_CXX_COMPILER}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Configuration" &&
        ${CMAKE_COMMAND} -E echo "  Name:               $<CONFIG>" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_DEBUG:   ${RAZ_CONFIG_DEBUG}" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_RELEASE: ${RAZ_CONFIG_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  RAZ_CONFIG_SHORT:   ${RAZ_CONFIG_SHORT}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Build flags: '${RAZ_BUILD_FLAGS}'" &&
        ${CMAKE_COMMAND} -E echo ""
    )
endif ()

if (RAZ_USE_EMSCRIPTEN)
    target_compile_definitions(RaZ PUBLIC RAZ_ROOT="/")
else ()
    target_compile_definitions(RaZ PUBLIC RAZ_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")
endif ()

########################
# RaZ - Compiler flags #
########################

include(CompilerFlags)
add_compiler_flags(RaZ PRIVATE)

if (RAZ_COMPILER_GCC)
    # Enabling code coverage
    option(RAZ_ENABLE_COVERAGE "Enable code coverage (GCC only)" OFF)

    if (RAZ_CONFIG_DEBUG AND RAZ_ENABLE_COVERAGE)
        target_compile_options(
            RaZ

            PRIVATE

            -g
            -O0
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
            -fprofile-arcs
            -ftest-coverage
        )

        target_link_libraries(RaZ PRIVATE gcov)
    endif ()
endif ()

if (RAZ_USE_EMSCRIPTEN)
    target_compile_options(
        RaZ

        PRIVATE

        "SHELL:-s USE_GLFW=3"
        "SHELL:-s USE_LIBPNG=1"
    )

    set(
        RAZ_LINKER_FLAGS

        ${RAZ_LINKER_FLAGS}
        "-s USE_LIBPNG=1"
        "-s USE_GLFW=3"
        "-s USE_WEBGL2=1"
        "-s OFFSCREEN_FRAMEBUFFER=1"
        "-s OFFSCREENCANVAS_SUPPORT=1"
        "-s DOUBLE_MODE=0"
        glfw
    )
endif ()

######################
# RaZ - Source files #
######################

set(
    RAZ_SRC

    src/RaZ/*.cpp
    src/RaZ/Animation/*.cpp
    src/RaZ/Audio/*.cpp
    src/RaZ/Math/*.cpp
    src/RaZ/Physics/*.cpp
    src/RaZ/Render/*.cpp
    src/RaZ/Utils/*.cpp

    include/RaZ/*.hpp
    include/RaZ/*.inl
    include/RaZ/Animation/*.hpp
    include/RaZ/Audio/*.hpp
    include/RaZ/Audio/*.inl
    include/RaZ/Math/*.hpp
    include/RaZ/Math/*.inl
    include/RaZ/Physics/*.hpp
    include/RaZ/Render/*.hpp
    include/RaZ/Render/*.inl
    include/RaZ/Utils/*.hpp
    include/RaZ/Utils/*.inl
)

# Adding every file to be compiled
file(
    GLOB
    RAZ_FILES

    ${RAZ_SRC}
)

# Defining platform-dependent options & linking flags
if (RAZ_PLATFORM_LINUX)
    option(RAZ_USE_SANITIZERS "Use sanitizers" OFF)

    if (RAZ_USE_SANITIZERS)
        target_compile_definitions(
            RaZ

            PRIVATE

            -fsanitize=undefined
            -fsanitize=address
            -fsanitize=leak
        )

        set(
            RAZ_LINKER_FLAGS

            ${RAZ_LINKER_FLAGS}
            asan
            ubsan
        )
    endif ()
endif ()

################
# RaZ - OpenAL #
################

find_package(OpenAL REQUIRED)

target_include_directories(RaZ SYSTEM PUBLIC ${OPENAL_INCLUDE_DIRS})
target_link_libraries(RaZ PUBLIC ${OPENAL_LIBS})

#########################
# RaZ - FBX file format #
#########################

if (RAZ_COMPILER_MSVC OR RAZ_COMPILER_GCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is considered as GCC
    option(RAZ_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" OFF)

    if (RAZ_USE_FBX)
        find_package(FBX)

        if (FBX_FOUND)
            set(RAZ_LINKER_FLAGS ${RAZ_LINKER_FLAGS} "${FBX_LIB}")

            target_compile_definitions(RaZ PUBLIC ${FBX_DEFINITIONS})
            target_include_directories(RaZ PUBLIC "${FBX_INCLUDE}")

            message(STATUS "[RaZ] FBX SDK linked (${FBX_LIB})")
        endif ()
    endif ()
endif ()

# If FBX SDK not used or found, do not compile FBX loader
if (NOT RAZ_USE_FBX OR NOT FBX_FOUND)
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/FbxImporter.cpp"
    )
endif ()

###############
# RaZ - Build #
###############

target_include_directories(RaZ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

option(SKIP_RENDERER_ERRORS "Do not print errors from the Renderer" OFF)
if (SKIP_RENDERER_ERRORS)
    target_compile_definitions(RaZ PRIVATE SKIP_RENDERER_ERRORS)
endif ()

option(RAZ_FORCE_DEBUG_LOG "Force the ouput of debug logging calls in non-Debug modes" OFF)
if (RAZ_FORCE_DEBUG_LOG)
    target_compile_definitions(RaZ PUBLIC RAZ_FORCE_DEBUG_LOG)
endif ()

# Choosing a rendering API
option(RAZ_USE_GL4 "Use OpenGL 4.5 for rendering" OFF)
if (RAZ_USE_GL4)
    if (RAZ_USE_VULKAN) # If Vulkan has also been asked to be used, don't do anything
        message(WARNING "[RaZ] Warning: Both Vulkan & OpenGL 4 have been defined to be used; only Vulkan will be.")
    else ()
        message(STATUS "[RaZ] Using OpenGL 4.5")
        target_compile_definitions(RaZ PRIVATE RAZ_USE_GL4)
    endif ()
endif ()

option(RAZ_USE_VULKAN "Use Vulkan for rendering" OFF)
if (RAZ_USE_VULKAN)
    if (RAZ_USE_GL4) # If GL4 has also been asked to be used, ignore it
        message(WARNING "[RaZ] Warning: Both Vulkan & OpenGL 4 have been defined to be used; only Vulkan will be.")
    endif ()

    ##############
    # Vulkan SDK #
    ##############

    if (NOT VULKAN_SDK_ROOT)
        # Vulkan SDK path not set, trying to find it automatically

        find_package(Vulkan)

        if (VULKAN_FOUND)
            # find_package() found Vulkan

            set(VULKAN_SDK_INCLUDE "${Vulkan_INCLUDE_DIRS}")
            set(VULKAN_SDK_LIB "${Vulkan_LIBRARIES}")

            set(GLSLC_EXECUTABLE glslc) # glslc is assumed to be available from the path... Kind of a long shot under Windows
        endif ()
    endif ()

    if (NOT VULKAN_FOUND)
        # If not found automatically by find_package(), try another way

        if (NOT VULKAN_SDK_ROOT AND RAZ_PLATFORM_WINDOWS)
            # If under Windows with the path not manually set, try to locate it from the environment variable
            set(VULKAN_SDK_ROOT "$ENV{VK_SDK_PATH}")
        endif ()

        if (VULKAN_SDK_ROOT)
            # SDK path has been defined

            if (RAZ_PLATFORM_WINDOWS)
                string(REPLACE "\\" "/" VULKAN_SDK_ROOT ${VULKAN_SDK_ROOT})
                set(VULKAN_SDK_INCLUDE "${VULKAN_SDK_ROOT}/Include")
                set(VULKAN_SDK_LIB "${VULKAN_SDK_ROOT}/Lib/vulkan-1.lib")
            else ()
                set(VULKAN_SDK_INCLUDE "${VULKAN_SDK_ROOT}/x86_64/include")
                set(VULKAN_SDK_LIB "${VULKAN_SDK_ROOT}/x86_64/lib/vulkan")
            endif ()

            find_program(GLSLC_EXECUTABLE glslc HINTS "${VULKAN_SDK_ROOT}/bin")

            if (EXISTS "${VULKAN_SDK_INCLUDE}/vulkan/vulkan.h" AND EXISTS "${VULKAN_SDK_LIB}")
                set(VULKAN_FOUND ON)
            endif ()
        endif ()
    endif ()

    if (VULKAN_FOUND)
        message(STATUS "[RaZ] Using Vulkan")
        if (VULKAN_SDK_ROOT)
            message(DEBUG "  - Root path: ${VULKAN_SDK_ROOT}")
        endif ()
        message(DEBUG "  - Include: ${VULKAN_SDK_INCLUDE}")
        message(DEBUG "  - Library: ${VULKAN_SDK_LIB}")
        if (GLSLC_EXECUTABLE)
            message(DEBUG "  - GLSLC executable: ${GLSLC_EXECUTABLE}")
        endif ()

        # Including & linking the Vulkan SDK

        target_include_directories(RaZ PUBLIC "${VULKAN_SDK_INCLUDE}")
        target_link_libraries(RaZ PUBLIC "${VULKAN_SDK_LIB}")

        target_compile_definitions(RaZ PUBLIC RAZ_USE_VULKAN)

        if (GLSLC_EXECUTABLE)
            #######################
            # Shaders compilation #
            #######################

            set(SHADERS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

            # Recovering shaders
            file(
                GLOB_RECURSE
                RAZ_SHADERS

                ${SHADERS_ROOT}/*_vk.vert # Vertex shader
                ${SHADERS_ROOT}/*_vk.tesc # Tesselation control shader
                ${SHADERS_ROOT}/*_vk.tese # Tesselation evaluation shader
                ${SHADERS_ROOT}/*_vk.geom # Geometry shader
                ${SHADERS_ROOT}/*_vk.frag # Fragment shader
                ${SHADERS_ROOT}/*_vk.comp # Compute shader
            )

            # Compiling all shaders found
            foreach (SHADER_INPUT_PATH ${RAZ_SHADERS})
                get_filename_component(SHADER_FILENAME ${SHADER_INPUT_PATH} NAME) # Stripping the path from the prepending folders, keeping the file's name
                string(REPLACE "." ";" SHADER_FILE_PARTS ${SHADER_FILENAME})

                list(GET SHADER_FILE_PARTS 0 SHADER_NAME)
                list(GET SHADER_FILE_PARTS 1 SHADER_EXT)

                set(SHADER_OUTPUT_PATH "${SHADERS_ROOT}/${SHADER_NAME}_${SHADER_EXT}.spv")

                add_custom_command(
                    OUTPUT "${SHADER_OUTPUT_PATH}"
                    COMMAND ${GLSLC_EXECUTABLE} ${SHADER_INPUT_PATH} -o ${SHADER_OUTPUT_PATH}
                    DEPENDS "${SHADER_INPUT_PATH}"
                    WORKING_DIRECTORY "${SHADERS_ROOT}"
                    COMMENT "Compiling shader ${SHADER_FILENAME} to SPIR-V"
                    VERBATIM
                )

                target_sources(RaZ PRIVATE "${SHADER_OUTPUT_PATH}")
            endforeach ()
        else ()
            # GLSLC executable not found, send a warning

            set(GLSLC_EXECUTABLE "" CACHE PATH "Path to your GLSLC executable")
            message(WARNING "[RaZ] Warning: GLSLC executable not found. To compile shaders into SPIR-V, please specify its path manually.")
        endif ()

        # Adding Vulkan-specific files
        file(
            GLOB
            RAZ_API_FILES

            ${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ/Render/Vulkan/*
            ${CMAKE_CURRENT_SOURCE_DIR}/src/RaZ/Render/Vulkan/*
        )
    else ()
        # Neither method has found a match, send an error

        set(VULKAN_SDK_ROOT "" CACHE PATH "Path to your Vulkan SDK installation")
        message(SEND_ERROR "[RaZ] Error: Vulkan SDK not found, please specify its installation path manually.")
    endif ()
else ()
    # Adding OpenGL-specific files
    file(
        GLOB
        RAZ_API_FILES

        ${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ/Render/OpenGL/*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RaZ/Render/OpenGL/*
    )
endif ()

list(
    APPEND
    RAZ_FILES

    ${RAZ_API_FILES}
)

target_link_libraries(RaZ PRIVATE ${RAZ_LINKER_FLAGS})

# Cygwin's Clang needs to use GCC's standard library
if (CYGWIN AND RAZ_COMPILER_CLANG)
    target_compile_options(RaZ PRIVATE -stdlib=libstdc++)
    target_link_libraries(RaZ PRIVATE stdc++)
endif ()

# Compiling & linking external libraries
add_subdirectory(extern)

if (RAZ_USE_LIBPNG)
    target_link_libraries(RaZ PRIVATE libpng)
endif ()

if (RAZ_USE_GLEW)
    target_link_libraries(RaZ PRIVATE GLEW)
endif ()

if (RAZ_USE_GLFW OR RAZ_USE_EMSCRIPTEN)
    if (NOT RAZ_USE_EMSCRIPTEN)
        # Emscripten handles GLFW on its own, but it is however not available directly; its link must be disabled
        target_link_libraries(RaZ PRIVATE GLFW)
    endif ()

    target_compile_definitions(RaZ PUBLIC RAZ_USE_WINDOW)
else ()
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/Window.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Utils/Window.hpp"
    )
endif ()

if (RAZ_USE_IMGUI)
    target_link_libraries(RaZ PRIVATE ImGui)
    target_compile_definitions(RaZ PUBLIC RAZ_USE_OVERLAY)
else ()
    list(
        REMOVE_ITEM
        RAZ_FILES

        "${PROJECT_SOURCE_DIR}/src/RaZ/Utils/Overlay.cpp"
        "${PROJECT_SOURCE_DIR}/include/RaZ/Utils/Overlay.cpp"
    )
endif ()

# Compiling RaZ's sources
target_sources(RaZ PRIVATE ${RAZ_FILES})

# Build the examples
option(RAZ_BUILD_EXAMPLES "Build examples" ON)
if (RAZ_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Build the unit tests
option(RAZ_BUILD_TESTS "Build unit tests" ON)
if (RAZ_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

# Allows to generate the documentation
find_package(Doxygen)
option(RAZ_GEN_DOC "Generate documentation (requires Doxygen)" ${DOXYGEN_FOUND})
if (RAZ_GEN_DOC)
    add_subdirectory(doc)
endif ()

######################
# RaZ - Installation #
######################

# Installing the library
if (RAZ_PLATFORM_WINDOWS)
    set(CMAKE_INSTALL_PREFIX "C:/RaZ")
endif ()

install(TARGETS RaZ DESTINATION "lib/$<CONFIG>")

# Installing the headers
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/RaZ" DESTINATION include)

# If FBX SDK used, install the DLL file into the bin/ folder
if (RAZ_COMPILER_MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    install(FILES "${FBX_CONFIG_DLL}" DESTINATION "bin/$<CONFIG>")
endif ()
