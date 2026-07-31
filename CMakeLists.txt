cmake_minimum_required(VERSION 3.16)
project(dk1_pc LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(dk1_core
    src/boot_control.c
    src/boot_memory.c
    src/boot_video.c
    src/camera.c
    src/frame_runtime.c
    src/host_callback.c
    src/input.c
    src/level_dispatch.c
    src/nmi_dispatch.c
    src/object_executor.c
    src/object_runtime.c
    src/object_scheduler.c
    src/ppu_scroll.c
    src/preview_actor.c
    src/render_queue.c
    src/reset_state.c
    src/rgba_surface.c
    src/rom_image.c
    src/snes_graphics.c
    src/state_sequence.c
    src/terrain.c
    src/tilemap_renderer.c
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

enable_testing()

foreach(test_name IN ITEMS
    reset
    boot_memory
    boot_control
    boot_video
    camera
    frame_runtime
    host_callback
    input
    level_dispatch
    nmi_dispatch
    object_executor
    object_runtime
    object_scheduler
    ppu_scroll
    preview_actor
    render_queue
    rgba_surface
    rom_image
    snes_graphics
    state_sequence
    terrain
    tilemap_renderer
)
    add_executable(test_${test_name} tests/test_${test_name}.c)
    target_link_libraries(test_${test_name} PRIVATE dk1_core)
    add_test(NAME ${test_name} COMMAND test_${test_name})
endforeach()

find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(
        NAME cfg65816
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cfg65816.py
    )
endif()
