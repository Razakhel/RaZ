#######
# stb #
#######

project(stb)

add_library(stb INTERFACE)

file(GLOB STB_FILES stb/*.h)

target_sources(stb INTERFACE ${STB_FILES})

target_include_directories(stb SYSTEM INTERFACE stb)
