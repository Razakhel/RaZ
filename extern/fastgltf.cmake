############
# fastgltf #
############

project(fastgltf)

add_library(fastgltf OBJECT)

target_compile_features(fastgltf PRIVATE cxx_std_17)

file(
    GLOB
    FASTGLTF_FILES

    fastgltf/src/*.cpp
    fastgltf/include/fastgltf/*.hpp
)

target_sources(fastgltf PRIVATE ${FASTGLTF_FILES})

target_include_directories(fastgltf SYSTEM PUBLIC fastgltf/include)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(fastgltf PRIVATE /w)
else ()
    target_compile_options(fastgltf PRIVATE -w)
endif ()

# Using simdjson
include(simdjson)
target_link_libraries(fastgltf PRIVATE simdjson)
