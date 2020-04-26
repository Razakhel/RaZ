##########
# libpng #
##########

project(libpng)

set(CMAKE_C_STANDARD 11)

aux_source_directory(png PNG_SRC)

# Building libpng
add_library(libpng OBJECT ${PNG_SRC})

target_include_directories(libpng SYSTEM PUBLIC png)

# Disabling all compilers warnings
if (MSVC)
    target_compile_options(libpng PRIVATE /w)
else ()
    target_compile_options(libpng PRIVATE -w)
endif ()
