############
# simdjson #
############

project(simdjson)

add_library(simdjson OBJECT)

target_compile_features(simdjson PRIVATE cxx_std_17)

set(
    SIMDJSON_FILES

    simdjson/simdjson.cpp
    simdjson/simdjson.h
)

target_sources(simdjson PRIVATE ${SIMDJSON_FILES})

target_include_directories(simdjson SYSTEM PUBLIC simdjson)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(simdjson PRIVATE /w)
else ()
    target_compile_options(simdjson PRIVATE -w)
endif ()
