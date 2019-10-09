##########
# libpng #
##########

project(libpng)

set(CMAKE_C_STANDARD 11)

aux_source_directory(png PNG_SRC)

# Building libpng
if (RAZ_BUILD_STATIC)
    add_library(libpng STATIC ${PNG_SRC})
else ()
    add_library(libpng SHARED ${PNG_SRC})
endif ()

target_include_directories(libpng SYSTEM PUBLIC png)

# Disabling all compilers warnings
if (MSVC)
    target_compile_options(libpng PRIVATE /w)
else ()
    target_compile_options(libpng PRIVATE -w)
endif ()
