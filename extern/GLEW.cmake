########
# GLEW #
########

project(GLEW)

set(CMAKE_C_STANDARD 11)

aux_source_directory(glew/src GLEW_SRC)

# Defining preprocessor macros
if (WIN32 OR CYGWIN)
    set(
        GLEW_DEFINITIONS

        -DGLEW_BUILD
        -DGLEW_NO_GLU
    )

    set(
        GLEW_LINKER_FLAGS

        opengl32
    )
endif ()

# Building GLEW
add_library(GLEW OBJECT ${GLEW_SRC})

if (CYGWIN)
    # Cygwin voluntarily removes the _WIN32 definition, which GLEW requires here
    target_compile_definitions(GLEW PRIVATE -D_WIN32)

    # Since ptrdiff_t is redefined with Clang, it needs some more tinkering...
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # ptrdiff_t is actually redefined by GLEW; _PTR_DIFF_T_ is added so that this doesn't happen
        # __need_ptrdiff_t is then defined so that the standard library can make the type available by itself
        target_compile_definitions(GLEW PRIVATE -D_PTRDIFF_T_ -D__need_ptrdiff_t)
    endif ()
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
