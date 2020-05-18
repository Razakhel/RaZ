##########
# libpng #
##########

project(libpng)

set(CMAKE_C_STANDARD 11)

set(
    PNG_SRC

    png/*.c
    png/*.h
)

file(
    GLOB
    PNG_FILES

    ${PNG_SRC}
)

# Building libpng
add_library(libpng OBJECT ${PNG_FILES})

target_include_directories(libpng SYSTEM PUBLIC png)

# Disabling all compilers warnings
if (MSVC)
    target_compile_options(libpng PRIVATE /w)
else ()
    target_compile_options(libpng PRIVATE -w)
endif ()
