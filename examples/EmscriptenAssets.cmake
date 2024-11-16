######################
# Assets - AudioDemo #
######################

if (RAZ_USE_AUDIO)
    set(
        RaZ_AudioDemo_ASSETS

        assets/sounds/knock.wav
        assets/sounds/wave_seagulls.wav
    )

    foreach (ASSET_PATH ${RaZ_AudioDemo_ASSETS})
        target_link_options(RaZ_AudioDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
    endforeach ()

    target_link_options(
        RaZ_AudioDemo

        PRIVATE

        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    )
endif ()

######################
# Assets - BloomDemo #
######################

set(
    RaZ_BloomDemo_ASSETS

    assets/meshes/ball.obj
    assets/materials/test.mtl
    assets/textures/rustediron_albedo.png
    assets/textures/rustediron_normal.png
    assets/textures/rustediron_metallic.png
    assets/textures/rustediron_roughness.png
)

foreach (ASSET_PATH ${RaZ_BloomDemo_ASSETS})
    target_link_options(RaZ_BloomDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_BloomDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

#########################
# Assets - DeferredDemo #
#########################

set(
    RaZ_DeferredDemo_ASSETS

    assets/meshes/shield.obj
    assets/materials/shield.mtl
    assets/textures/shield_albedo.png
    assets/textures/shield_normal.png
    assets/textures/shield_metallic.png
    assets/textures/shield_roughness.png
    assets/textures/shield_ao.png
)

foreach (ASSET_PATH ${RaZ_DeferredDemo_ASSETS})
    target_link_options(RaZ_DeferredDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_DeferredDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

#####################
# Assets - FullDemo #
#####################

set(
    RaZ_FullDemo_ASSETS

    assets/icons/RaZ_logo_128.png
    assets/meshes/shield.obj
    assets/materials/shield.mtl
    assets/skyboxes/clouds_right.png
    assets/skyboxes/clouds_top.png
    assets/skyboxes/clouds_front.png
    assets/skyboxes/clouds_left.png
    assets/skyboxes/clouds_bottom.png
    assets/skyboxes/clouds_back.png
    assets/sounds/wave_seagulls.wav
    assets/textures/shield_albedo.png
    assets/textures/shield_normal.png
    assets/textures/shield_metallic.png
    assets/textures/shield_roughness.png
    assets/textures/shield_ao.png
)

foreach (ASSET_PATH ${RaZ_FullDemo_ASSETS})
    target_link_options(RaZ_FullDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_FullDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

####################
# Assets - MinDemo #
####################

set(
    RaZ_MinDemo_ASSETS

    assets/meshes/ball.obj
    assets/materials/test.mtl
    assets/textures/rustediron_albedo.png
    assets/textures/rustediron_normal.png
    assets/textures/rustediron_metallic.png
    assets/textures/rustediron_roughness.png
)

foreach (ASSET_PATH ${RaZ_MinDemo_ASSETS})
    target_link_options(RaZ_MinDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_MinDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

########################
# Assets - PhysicsDemo #
########################

set(
    RaZ_PhysicsDemo_ASSETS

    assets/meshes/ball.obj
    assets/materials/test.mtl
    assets/textures/rustediron_albedo.png
    assets/textures/rustediron_normal.png
    assets/textures/rustediron_metallic.png
    assets/textures/rustediron_roughness.png
)

foreach (ASSET_PATH ${RaZ_PhysicsDemo_ASSETS})
    target_link_options(RaZ_PhysicsDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_PhysicsDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

#######################
# Assets - ScriptDemo #
#######################

if (RAZ_USE_LUA)
    set(
        RaZ_ScriptDemo_ASSETS

        assets/meshes/shield.obj
        assets/materials/shield.mtl
        assets/textures/shield_albedo.png
        assets/textures/shield_normal.png
        assets/textures/shield_metallic.png
        assets/textures/shield_roughness.png
        assets/textures/shield_ao.png
    )

    foreach (ASSET_PATH ${RaZ_ScriptDemo_ASSETS})
        target_link_options(RaZ_ScriptDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
    endforeach ()

    target_link_options(
        RaZ_ScriptDemo

        PRIVATE

        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/scripts@scripts"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    )
endif ()

####################
# Assets - SSRDemo #
####################

set(
    RaZ_SSRDemo_ASSETS

    assets/textures/checkerboard.png
    assets/textures/rustediron_normal.png
)

foreach (ASSET_PATH ${RaZ_SSRDemo_ASSETS})
    target_link_options(RaZ_SSRDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_SSRDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)
