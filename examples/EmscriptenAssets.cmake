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

#########################
# Assets - ShowcaseDemo #
#########################

set(
    RaZ_ShowcaseDemo_ASSETS

    # Skyboxes
    assets/skyboxes/clouds_right.png
    assets/skyboxes/clouds_top.png
    assets/skyboxes/clouds_front.png
    assets/skyboxes/clouds_left.png
    assets/skyboxes/clouds_bottom.png
    assets/skyboxes/clouds_back.png

    assets/skyboxes/lake_right.png
    assets/skyboxes/lake_top.png
    assets/skyboxes/lake_front.png
    assets/skyboxes/lake_left.png
    assets/skyboxes/lake_bottom.png
    assets/skyboxes/lake_back.png

    # Shield mesh
    assets/meshes/shield.obj
    assets/materials/shield.mtl
    assets/textures/shield_albedo.png
    assets/textures/shield_normal.png
    assets/textures/shield_metallic.png
    assets/textures/shield_roughness.png
    assets/textures/shield_ao.png

    # Crytek Sponza mesh
    assets/meshes/crytek_sponza.obj
    assets/materials/crytek_sponza.mtl

    assets/textures/sponza_thorn_diff.png
    assets/textures/sponza_thorn_mask.png
    assets/textures/sponza_thorn_bump.png

    assets/textures/vase_round.png
    assets/textures/vase_round_bump.png

    assets/textures/vase_plant.png
    assets/textures/vase_plant_mask.png

    assets/textures/background.png
    assets/textures/background_bump.png

    assets/textures/spnza_bricks_a_spec.png
    assets/textures/spnza_bricks_a_diff.png
    assets/textures/spnza_bricks_a_bump.png

    assets/textures/sponza_arch_diff.png

    assets/textures/sponza_ceiling_a_diff.png

    assets/textures/sponza_column_a_diff.png
    assets/textures/sponza_column_a_bump.png

    assets/textures/sponza_floor_a_diff.png

    assets/textures/sponza_column_c_diff.png
    assets/textures/sponza_column_c_bump.png

    assets/textures/sponza_details_diff.png

    assets/textures/sponza_column_b_diff.png
    assets/textures/sponza_column_b_bump.png

    assets/textures/sponza_flagpole_diff.png

    assets/textures/sponza_fabric_green_diff.png

    assets/textures/sponza_fabric_blue_diff.png

    assets/textures/sponza_fabric_diff.png

    assets/textures/sponza_curtain_blue_diff.png

    assets/textures/sponza_curtain_diff.png

    assets/textures/sponza_curtain_green_diff.png

    assets/textures/chain_texture.png
    assets/textures/chain_texture_mask.png
    assets/textures/chain_texture_bump.png

    assets/textures/vase_hanging.png

    assets/textures/vase_dif.png
    assets/textures/vase_bump.png

    assets/textures/lion.png
    assets/textures/lion_bump.png

    assets/textures/sponza_roof_diff.png

    # Ball mesh
    assets/meshes/ball.obj
    assets/materials/test.mtl

    assets/textures/rustediron_albedo.png
    assets/textures/rustediron_normal.png
    assets/textures/rustediron_metallic.png
    assets/textures/rustediron_roughness.png

    # Cerberus mesh
    assets/meshes/cerberus.obj
    assets/materials/cerberus.mtl

    assets/textures/cerberus_albedo.png
    assets/textures/cerberus_normal.png
    assets/textures/cerberus_metallic.png
    assets/textures/cerberus_roughness.png
    assets/textures/cerberus_ao.png
)

foreach (ASSET_PATH ${RaZ_ShowcaseDemo_ASSETS})
    target_link_options(RaZ_ShowcaseDemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_ShowcaseDemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

#####################
# Assets - SSAODemo #
#####################

set(
    RaZ_SSAODemo_ASSETS

    assets/meshes/crytek_sponza.obj
    assets/materials/crytek_sponza.mtl

    assets/textures/sponza_thorn_diff.png
    assets/textures/sponza_thorn_mask.png
    assets/textures/sponza_thorn_bump.png

    assets/textures/vase_round.png
    assets/textures/vase_round_bump.png

    assets/textures/vase_plant.png
    assets/textures/vase_plant_mask.png

    assets/textures/background.png
    assets/textures/background_bump.png

    assets/textures/spnza_bricks_a_spec.png
    assets/textures/spnza_bricks_a_diff.png
    assets/textures/spnza_bricks_a_bump.png

    assets/textures/sponza_arch_diff.png

    assets/textures/sponza_ceiling_a_diff.png

    assets/textures/sponza_column_a_diff.png
    assets/textures/sponza_column_a_bump.png

    assets/textures/sponza_floor_a_diff.png

    assets/textures/sponza_column_c_diff.png
    assets/textures/sponza_column_c_bump.png

    assets/textures/sponza_details_diff.png

    assets/textures/sponza_column_b_diff.png
    assets/textures/sponza_column_b_bump.png

    assets/textures/sponza_flagpole_diff.png

    assets/textures/sponza_fabric_green_diff.png

    assets/textures/sponza_fabric_blue_diff.png

    assets/textures/sponza_fabric_diff.png

    assets/textures/sponza_curtain_blue_diff.png

    assets/textures/sponza_curtain_diff.png

    assets/textures/sponza_curtain_green_diff.png

    assets/textures/chain_texture.png
    assets/textures/chain_texture_mask.png
    assets/textures/chain_texture_bump.png

    assets/textures/vase_hanging.png

    assets/textures/vase_dif.png
    assets/textures/vase_bump.png

    assets/textures/lion.png
    assets/textures/lion_bump.png

    assets/textures/sponza_roof_diff.png
)

foreach (ASSET_PATH ${RaZ_SSAODemo_ASSETS})
    target_link_options(RaZ_SSAODemo PRIVATE "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/${ASSET_PATH}@${ASSET_PATH}")
endforeach ()

target_link_options(
    RaZ_SSAODemo

    PRIVATE

    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders@shaders"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
)

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
