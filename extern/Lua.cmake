#######
# Lua #
#######

project(Lua)

add_library(Lua OBJECT)

target_compile_features(Lua PRIVATE c_std_11)

file(
    GLOB
    LUA_FILES

    lua/*.c
    lua/*.h
)

# Removing unwanted files just in case
list(
    REMOVE_ITEM
    LUA_FILES

    "${CMAKE_CURRENT_LIST_DIR}/lua/linit.c" # Initialization for the standalone interpreter
    "${CMAKE_CURRENT_LIST_DIR}/lua/lua.c"   # Standalone interpreter
    "${CMAKE_CURRENT_LIST_DIR}/lua/luac.c"  # Compiler
)

target_sources(Lua PRIVATE ${LUA_FILES})

target_include_directories(Lua SYSTEM PUBLIC lua)

# Disabling all compilers warnings
if (RAZ_COMPILER_MSVC)
    target_compile_options(Lua PRIVATE /w)
else ()
    target_compile_options(Lua PRIVATE -w)
endif ()
