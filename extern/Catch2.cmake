##########
# Catch2 #
##########

project(Catch2)

add_library(Catch2 STATIC)

target_compile_features(Catch2 PRIVATE cxx_std_17)

# Configuring the user config manually, as it is done on Catch's side

# Declaring parameters needed for the configuration; see Catch's doc if more may need to be set
set(CATCH_CONFIG_DEFAULT_REPORTER "console" CACHE STRING "")
set(CATCH_CONFIG_CONSOLE_WIDTH "80" CACHE STRING "")

set(CATCH_USER_CONFIG_PATH "${CMAKE_BINARY_DIR}/generated-includes/catch2/catch_user_config.hpp")

configure_file(
    "catch/catch2/catch_user_config.hpp.in"
    "${CATCH_USER_CONFIG_PATH}"
)

file(
    GLOB_RECURSE
    CATCH2_FILES

    catch/catch2/*.cpp
    catch/catch2/*.hpp
    "${CATCH_USER_CONFIG_PATH}"
)

target_sources(Catch2 PRIVATE ${CATCH2_FILES})

target_include_directories(
    Catch2

    SYSTEM
    PUBLIC
        catch
        "${CMAKE_BINARY_DIR}/generated-includes"
)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(Catch2 PRIVATE /w)
else ()
    target_compile_options(Catch2 PRIVATE -w)
endif ()
