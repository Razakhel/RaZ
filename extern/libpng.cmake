##########
# libpng #
##########

project(libpng)

add_library(libpng OBJECT)

target_compile_features(libpng PRIVATE c_std_11)

file(
    GLOB
    PNG_FILES

    png/*.c
    png/*.h
)

# Building libpng
target_sources(libpng PRIVATE ${PNG_FILES})

target_include_directories(libpng SYSTEM PUBLIC png)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(libpng PRIVATE /w)
else ()
    target_compile_options(libpng PRIVATE -w)
endif ()
