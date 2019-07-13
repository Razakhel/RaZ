project(RaZ_Examples)

set(CMAKE_CXX_STANDARD 17)

add_executable(RaZ_FullDemo fullDemo.cpp)
target_link_libraries(RaZ_FullDemo RaZ)

add_executable(RaZ_MinDemo minDemo.cpp)
target_link_libraries(RaZ_MinDemo RaZ)

add_executable(RaZ_ShowcaseDemo showcaseDemo.cpp)
target_link_libraries(RaZ_ShowcaseDemo RaZ)

# If FBX SDK used with MSVC, copy the DLL into the examples/ folder to link it properly
if (MSVC AND RAZ_USE_FBX AND FBX_FOUND)
    set(DLL_COPY_COMMAND ${CMAKE_COMMAND} -E copy "${FBX_CONFIG_DLL}" "${CMAKE_BINARY_DIR}/examples/${FBX_LIB_NAME}.dll")

    add_custom_command(TARGET RaZ_FullDemo     PRE_LINK COMMAND ${DLL_COPY_COMMAND})
    add_custom_command(TARGET RaZ_MinDemo      PRE_LINK COMMAND ${DLL_COPY_COMMAND})
    add_custom_command(TARGET RaZ_ShowcaseDemo PRE_LINK COMMAND ${DLL_COPY_COMMAND})
endif ()
