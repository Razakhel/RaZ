#########
# Tracy #
#########

project(Tracy)

add_library(Tracy STATIC)

target_compile_features(Tracy PRIVATE cxx_std_17)

target_sources(Tracy PRIVATE tracy/TracyClient.cpp)

target_include_directories(Tracy SYSTEM PUBLIC tracy)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(Tracy PRIVATE /w)
else ()
    target_compile_options(Tracy PRIVATE -w)
endif ()
