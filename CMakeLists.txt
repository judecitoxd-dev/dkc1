cmake_minimum_required(VERSION 3.16)
project(dk1_pc LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(dk1_core
    src/player_advanced_runtime.c
    src/apu_driver_catalog.c
    src/player_extended_runtime.c
    src/object_animation_script.c
    src/spc700_bootstrap.c
    src/player_state_runtime.c
    src/player_interrupt.c
    src/player_input_router.c
    src/player_coverage.c
    src/dynamic_tile_stream.c
    src/apu_boot_image.c
    src/apu_mailbox.c
    src/bg_renderer.c
    src/boot_control.c
    src/boot_memory.c
    src/boot_video.c
    src/camera.c
    src/cgram_image.c
    src/dkc_decompress.c
    src/dynamic_stream.c
    src/frame_replay.c
    src/frame_runtime.c
    src/host_callback.c
    src/host_input.c
    src/input.c
    src/level_camera_bounds.c
    src/level_diagnostic.c
    src/level_dispatch.c
    src/level_map.c
    src/level_metatile.c
    src/level_profile_map.c
    src/level_terrain_config.c
    src/nmi_dispatch.c
    src/oam_animation.c
    src/oam_image.c
    src/object_executor.c
    src/object_runtime.c
    src/object_scheduler.c
    src/player_common.c
    src/player_dispatch.c
    src/player_events.c
    src/player_state_plan.c
    src/ppu_bg_renderer.c
    src/ppu_compositor.c
    src/ppu_effects.c
    src/ppu_layout.c
    src/ppu_preset.c
    src/ppu_scroll.c
    src/preview_actor.c
    src/render_queue.c
    src/reset_state.c
    src/rgba_surface.c
    src/rom_image.c
    src/rom_terrain.c
    src/save_state.c
    src/scene_memory.c
    src/scene_package_plan.c
    src/scene_recipe.c
    src/scene_runtime.c
    src/scene_signature.c
    src/scene_view.c
    src/snes_graphics.c
    src/software_frontend.c
    src/state_sequence.c
    src/terrain.c
    src/terrain_contacts.c
    src/tilemap_renderer.c
    src/vram_image.c
    src/vram_package.c
)
target_include_directories(dk1_core PUBLIC include)

if(MSVC)
    target_compile_options(dk1_core PRIVATE /W4)
else()
    target_compile_options(dk1_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(dk1_pc src/main.c)
target_link_libraries(dk1_pc PRIVATE dk1_core)
add_executable(dk1_asset_probe src/asset_probe.c)
target_link_libraries(dk1_asset_probe PRIVATE dk1_core)
add_executable(dk1_level_probe src/level_probe.c)
target_link_libraries(dk1_level_probe PRIVATE dk1_core)
add_executable(dk1_vram_probe src/vram_probe.c)
target_link_libraries(dk1_vram_probe PRIVATE dk1_core)
add_executable(dk1_scene_probe src/scene_probe.c)
target_link_libraries(dk1_scene_probe PRIVATE dk1_core)
add_executable(dk1_scene_validate src/scene_validate.c)
target_link_libraries(dk1_scene_validate PRIVATE dk1_core)
add_executable(dk1_frontend_probe src/frontend_probe.c)
target_link_libraries(dk1_frontend_probe PRIVATE dk1_core)
add_executable(dk1_frontend_validate src/frontend_validate.c)
target_link_libraries(dk1_frontend_validate PRIVATE dk1_core)
add_executable(dk1_player_validate src/player_validate.c)
target_link_libraries(dk1_player_validate PRIVATE dk1_core)
add_executable(dk1_gameplay_validate src/gameplay_validate.c)
target_link_libraries(dk1_gameplay_validate PRIVATE dk1_core)

enable_testing()

foreach(test_name IN ITEMS
    player_advanced_runtime
    apu_driver_catalog
    player_extended_runtime
    object_animation_script
    spc700_bootstrap
    player_state_runtime
    player_interrupt
    player_input_router
    player_coverage
    dynamic_tile_stream
    apu_boot_image
    reset
    apu_mailbox
    bg_renderer
    boot_memory
    boot_control
    boot_video
    camera
    cgram_image
    dkc_decompress
    dynamic_stream
    frame_replay
    frame_runtime
    host_callback
    host_input
    input
    level_camera_bounds
    level_diagnostic
    level_dispatch
    level_map
    level_map_visual
    level_metatile
    level_profile_map
    level_terrain_config
    nmi_dispatch
    oam_animation
    oam_image
    object_executor
    object_runtime
    object_scheduler
    player_common
    player_dispatch
    player_events
    player_state_plan
    ppu_bg_renderer
    ppu_compositor
    ppu_effects
    ppu_layout
    ppu_preset
    ppu_scroll
    preview_actor
    render_queue
    rgba_surface
    rom_image
    rom_terrain
    save_state
    scene_package_plan
    scene_recipe
    scene_runtime
    scene_signature
    scene_view
    snes_graphics
    snes_graphics_depth
    software_frontend
    state_sequence
    terrain
    terrain_contacts
    tilemap_renderer
    vram_image
    vram_package
)
    add_executable(test_${test_name} tests/test_${test_name}.c)
    target_link_libraries(test_${test_name} PRIVATE dk1_core)
    add_test(NAME ${test_name} COMMAND test_${test_name})
endforeach()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME cfg65816 COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cfg65816.py)
endif()

find_package(X11)
if(X11_FOUND)
    add_executable(dk1_x11 src/x11_frontend.c)
    target_include_directories(dk1_x11 PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(dk1_x11 PRIVATE dk1_core ${X11_LIBRARIES})
endif()
