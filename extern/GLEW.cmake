########
# GLEW #
########

project(GLEW)

set(CMAKE_C_STANDARD 11)

aux_source_directory(glew/src GLEW_SRC)

# Defining preprocessor macros
if (WIN32)
    set(
        GLEW_DEFINITIONS

        -DGLEW_BUILD
        #-DGLEW_NO_GLU
    )

    set(
        GLEW_LINKER_FLAGS

        opengl32
    )
endif ()

# Building GLEW
if (RAZ_BUILD_STATIC)
    add_library(GLEW STATIC ${GLEW_SRC})
else ()
    add_library(GLEW SHARED ${GLEW_SRC})
endif ()

target_include_directories(GLEW SYSTEM PUBLIC glew/include)

target_compile_definitions(GLEW PUBLIC ${GLEW_DEFINITIONS})

# Disabling all compilers warnings
if (MSVC)
    target_compile_options(GLEW PRIVATE /w)
else ()
    target_compile_options(GLEW PRIVATE -w)
endif ()

target_link_libraries(GLEW PRIVATE ${GLEW_LINKER_FLAGS})
