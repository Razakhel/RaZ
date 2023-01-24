########
# Sol2 #
########

project(Sol2)

add_library(Sol2 INTERFACE)

file(GLOB SOL2_FILES sol/*.hpp)

# Adding Sol2's headers as sources
target_sources(Sol2 INTERFACE ${SOL2_FILES})

target_include_directories(Sol2 SYSTEM INTERFACE sol)

# Using Lua
include(Lua)
target_link_libraries(Sol2 INTERFACE Lua)
