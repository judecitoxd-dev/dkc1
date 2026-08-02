#define WinMain dk1_shell_preview_WinMain
#include "win32_frontend.c"
#undef WinMain
#include "dk1/first_level_route.h"

/* A deliberately narrow tester for the first playable delivery. It reuses the
 * normal ROM validation, cache, startup texture warmup, input and renderer, but
 * enters Jungle Hijinxs immediately instead of waiting through the provisional
 * intro/menu/map shell. */

static bool restart_direct_test(Dk1Win32App *app,
                                Dk1FirstLevelRoute *route) {
    if (app == NULL || route == NULL || !reset_level_runtime(app))
        return false;
    if (!dk1_first_level_route_init(
            app->scene, (uint16_t)app->width,
            app->frontend.player_preview.motion.world_x, route))
        return false;
    app->held = 0u;
    app->previous_held = 0u;
    app->route_completed = false;
    app->level_preloaded = false;
    return true;
}

static bool player_below_test_envelope(const Dk1Win32App *app) {
    int32_t screen_y;
    if (app == NULL)
        return false;
    screen_y = (int32_t)app->frontend.player_vertical_origin -
               (int32_t)app->frontend.runtime.view.camera_y -
               (int32_t)app->frontend.player_preview.motion.world_y;
    return screen_y > (int32_t)app->height + 96;
}

static bool warp_direct_test(Dk1Win32App *app,
                             Dk1FirstLevelRoute *route,
                             uint8_t target_percent) {
    Dk1PlayerPreviewRuntime *player;
    uint32_t distance;
    uint32_t target_x;
    uint32_t camera_x;
    if (app == NULL || route == NULL || app->scene == NULL ||
        !app->frontend.player_terrain_ready || target_percent > 99u)
        return false;
    distance = route->exit_x - route->start_x;
    target_x = route->start_x + distance * target_percent / 100u;
    if (target_x >= route->exit_x)
        target_x = route->exit_x - 1u;
    if (target_x > UINT16_MAX)
        return false;

    player = &app->frontend.player_preview;
    player->motion.world_x = (uint16_t)target_x;
    player->motion.subpixel_x = 0u;
    player->motion.subpixel_y = 0u;
    player->motion.velocity_x = 0;
    player->motion.velocity_y = 0;
    player->motion.base_velocity_x = 0;
    player->motion.target_velocity_x = 0;
    player->airborne = true;
    if (!dk1_player_terrain_place(
            &app->frontend.player_terrain, player,
            (uint16_t)app->height,
            &app->frontend.runtime.view.camera_y))
        return false;

    camera_x = target_x > (uint32_t)app->width / 2u
        ? target_x - (uint32_t)app->width / 2u
        : 0u;
    if (camera_x > UINT16_MAX)
        camera_x = UINT16_MAX;
    app->frontend.runtime.view.camera_x = (uint16_t)camera_x;
    if (!dk1_scene_view_clamp(
            app->scene, &app->frontend.runtime.view))
        return false;
    dk1_first_level_route_step(route, target_x);
    app->route_completed = route->completed;
    return true;
}

static void write_preflight_report(
    const Dk1Win32App *app,
    const Dk1FirstLevelRoute *route,
    const Dk1FirstLevelPreflight *preflight) {
    FILE *file;
    if (app == NULL || route == NULL || preflight == NULL)
        return;
    file = fopen("DK1-Jungle-Hijinxs-Preflight.txt", "w");
    if (file == NULL)
        return;
    fprintf(file, "DK1 Jungle Hijinxs startup preflight\n");
    fprintf(file, "ready=%u\n", preflight->ready ? 1u : 0u);
    fprintf(file, "failure_mask=0x%08lX\n",
            (unsigned long)preflight->failure_mask);
    fprintf(file, "scene_packages=%llu\n",
            (unsigned long long)preflight->scene_packages);
    fprintf(file, "scene_dma_bytes=%llu\n",
            (unsigned long long)preflight->scene_dma_bytes);
    fprintf(file, "initial_map_columns=%llu\n",
            (unsigned long long)preflight->initial_map_columns);
    fprintf(file, "initial_tile_words=%llu\n",
            (unsigned long long)preflight->initial_tile_words);
    fprintf(file, "player_dma_records=%llu\n",
            (unsigned long long)preflight->player_dma_records);
    fprintf(file, "player_dma_bytes=%llu\n",
            (unsigned long long)preflight->player_dma_bytes);
    fprintf(file, "route_start_x=%lu\n", (unsigned long)route->start_x);
    fprintf(file, "route_exit_x=%lu\n", (unsigned long)route->exit_x);
    fprintf(file, "route_distance=%lu\n",
            (unsigned long)preflight->route_distance);
    fprintf(file, "scene_cache_hit=%u\n",
            app->scene != NULL && app->scene->cache.hit ? 1u : 0u);
    fprintf(file, "textures_loaded=%u\n",
            app->scene != NULL && app->scene->assets.textures_loaded ? 1u : 0u);
    fprintf(file, "palettes_loaded=%u\n",
            app->scene != NULL && app->scene->assets.palettes_loaded ? 1u : 0u);
    fprintf(file, "warmup_player_visual=%u\n",
            app->warmup_stats.player_visual_ready ? 1u : 0u);
    fprintf(file, "warmup_dynamic_bg1=%u\n",
            app->warmup_stats.dynamic_bg1_ready ? 1u : 0u);
    fclose(file);
}

static void write_direct_test_report(
    const Dk1Win32App *app,
    const Dk1FirstLevelRoute *route,
    const Dk1FirstLevelPreflight *preflight,
    uint32_t manual_restarts,
    uint32_t fall_restarts,
    uint32_t debug_warps) {
    FILE *file;
    uint32_t bg1_generation = 0u;
    size_t bg1_columns = 0u;
    if (app == NULL || route == NULL || preflight == NULL)
        return;
    if (app->frontend.dynamic_bg1 != NULL) {
        bg1_generation = app->frontend.dynamic_bg1->generation;
        bg1_columns = app->frontend.dynamic_bg1->map_columns_uploaded;
    }
    file = fopen("DK1-Jungle-Hijinxs-Test-Report.txt", "w");
    if (file == NULL)
        return;
    fprintf(file, "DK1 Jungle Hijinxs direct test report\n");
    fprintf(file, "preflight_ready=%u\n", preflight->ready ? 1u : 0u);
    fprintf(file, "preflight_failure_mask=0x%08lX\n",
            (unsigned long)preflight->failure_mask);
    fprintf(file, "route_progress=%u\n",
            (unsigned)route->progress_percent);
    fprintf(file, "route_completed=%u\n", route->completed ? 1u : 0u);
    fprintf(file, "route_frames=%llu\n",
            (unsigned long long)route->frames);
    fprintf(file, "route_start_x=%lu\n", (unsigned long)route->start_x);
    fprintf(file, "route_furthest_x=%lu\n",
            (unsigned long)route->furthest_x);
    fprintf(file, "route_exit_x=%lu\n", (unsigned long)route->exit_x);
    fprintf(file, "checkpoints_reached=%u\n",
            (unsigned)route->checkpoints_reached);
    fprintf(file, "manual_restarts=%lu\n",
            (unsigned long)manual_restarts);
    fprintf(file, "fall_restarts=%lu\n",
            (unsigned long)fall_restarts);
    fprintf(file, "debug_warps=%lu\n",
            (unsigned long)debug_warps);
    fprintf(file, "textures_preloaded=%u\n",
            app->startup_visuals_warmed ? 1u : 0u);
    fprintf(file, "scene_cache_hit=%u\n", app->scene != NULL &&
            app->scene->cache.hit ? 1u : 0u);
    fprintf(file, "dynamic_bg1_generation=%lu\n",
            (unsigned long)bg1_generation);
    fprintf(file, "dynamic_bg1_map_columns=%llu\n",
            (unsigned long long)bg1_columns);
    fprintf(file, "active_gnawties=%llu\n",
            (unsigned long long)app->frontend.gnawty_active_count);
    fprintf(file, "active_streamed_barrels=%llu\n",
            (unsigned long long)app->frontend.streamed_barrels.active_count);
    fclose(file);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                   LPSTR command_line, int show_command) {
    Dk1Win32App app;
    Dk1RgbaSurface surface;
    Dk1FirstLevelRoute route;
    Dk1FirstLevelPreflight preflight;
    MSG message;
    char rom_path[MAX_PATH];
    ULONGLONG next_frame;
    ULONGLONG title_time;
    uint32_t manual_restarts = 0u;
    uint32_t fall_restarts = 0u;
    uint32_t debug_warps = 0u;
    int rom_argument = 1;
    int result = 1;
    bool completion_reported = false;
    bool preflight_only = false;
    (void)previous_instance;
    (void)command_line;
    (void)show_command;

    memset(&app, 0, sizeof(app));
    memset(&route, 0, sizeof(route));
    memset(&preflight, 0, sizeof(preflight));
    app.level = DK1_DEMO_LEVEL;
    app.width = DK1_DEMO_WIDTH;
    app.height = DK1_DEMO_HEIGHT;
    app.running = true;
    g_app = &app;

    if (__argc >= 2 && strcmp(__argv[1], "--preflight") == 0) {
        preflight_only = true;
        rom_argument = 2;
    }
    if (__argc > rom_argument) {
        strncpy(rom_path, __argv[rom_argument], MAX_PATH - 1u);
        rom_path[MAX_PATH - 1u] = '\0';
    } else if (!choose_rom(rom_path)) {
        g_app = NULL;
        return 0;
    }

    if (!initialize_game(&app, rom_path)) {
        MessageBoxA(NULL,
            "Could not preload Jungle Hijinxs. Select an unheadered USA Rev 2 ROM.",
            "DK1 level test", MB_ICONERROR);
        goto cleanup;
    }
    if (!dk1_first_level_route_init(
            app.scene, (uint16_t)app.width,
            app.frontend.player_preview.motion.world_x, &route)) {
        MessageBoxA(NULL, "Could not initialize the first-level test route.",
                    "DK1 level test", MB_ICONERROR);
        goto cleanup;
    }
    if (!dk1_first_level_preflight(
            app.scene, &app.frontend, &app.warmup_stats,
            &route, &preflight)) {
        char text[320];
        write_preflight_report(&app, &route, &preflight);
        snprintf(text, sizeof(text),
                 "Jungle Hijinxs startup preflight failed (mask 0x%08lX). "
                 "See DK1-Jungle-Hijinxs-Preflight.txt beside the executable.",
                 (unsigned long)preflight.failure_mask);
        MessageBoxA(NULL, text, "DK1 level test", MB_ICONERROR);
        goto cleanup;
    }
    write_preflight_report(&app, &route, &preflight);
    if (preflight_only) {
        MessageBoxA(NULL,
                    "Jungle Hijinxs startup preflight passed. Textures, "
                    "palettes, terrain, objects, dynamic background and the "
                    "first player frame are ready. The report was written "
                    "beside the executable.",
                    "DK1 preflight passed", MB_OK | MB_ICONINFORMATION);
        result = 0;
        goto cleanup;
    }

    app.flow.state = DK1_PREVIEW_FLOW_LEVEL;
    app.flow.previous_state = DK1_PREVIEW_FLOW_MAP;
    app.flow.state_frames = 0u;
    app.flow.state_changed = false;
    app.flow.level_entered = true;
    app.level_preloaded = false;

    if (!create_window(&app, instance)) {
        MessageBoxA(NULL, "Could not create the Jungle Hijinxs test window.",
                    "DK1 level test", MB_ICONERROR);
        goto cleanup;
    }
    SetWindowTextA(app.window,
                   "DK1 Jungle Hijinxs direct test - preflight passed");

    surface = (Dk1RgbaSurface){app.pixels, (size_t)app.width,
                               (size_t)app.height, (size_t)app.width};
    next_frame = GetTickCount64();
    title_time = next_frame;
    while (app.running) {
        uint16_t pressed;
        bool restart_requested;
        bool warp_requested;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT)
                app.running = false;
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        if (!app.running)
            break;

        pressed = (uint16_t)(app.held & (uint16_t)~app.previous_held);
        app.previous_held = app.held;
        restart_requested =
            (GetAsyncKeyState('R') & 1) != 0 ||
            (route.completed &&
             (pressed & DK1_HOST_BUTTON_START) != 0u);
        warp_requested =
            !route.completed && (GetAsyncKeyState(VK_F2) & 1) != 0;
        if (restart_requested) {
            ++manual_restarts;
            if (!restart_direct_test(&app, &route)) {
                MessageBoxA(app.window,
                            "Jungle Hijinxs could not be restarted.",
                            "DK1 level test", MB_ICONERROR);
                break;
            }
            completion_reported = false;
            next_frame = GetTickCount64();
            continue;
        }
        if (warp_requested) {
            uint8_t target_percent =
                (uint8_t)(route.progress_percent + 10u);
            if (target_percent > 95u)
                target_percent = 95u;
            if (target_percent > route.progress_percent) {
                if (!warp_direct_test(&app, &route, target_percent)) {
                    MessageBoxA(app.window,
                                "The F2 test warp could not find valid terrain.",
                                "DK1 level test", MB_ICONWARNING);
                } else {
                    ++debug_warps;
                    next_frame = GetTickCount64();
                    continue;
                }
            }
        }

        if (!route.completed) {
            if (!dk1_software_frontend_step(
                    app.scene, app.held, &app.frontend) ||
                !dk1_software_frontend_render(
                    &app.rom, app.scene, &app.frontend, surface)) {
                MessageBoxA(app.window,
                            "The direct level test stopped because a frame failed.",
                            "DK1 level test", MB_ICONERROR);
                break;
            }

            if (player_below_test_envelope(&app)) {
                ++fall_restarts;
                if (!restart_direct_test(&app, &route)) {
                    MessageBoxA(app.window,
                                "The player fell and the level could not restart.",
                                "DK1 level test", MB_ICONERROR);
                    break;
                }
                completion_reported = false;
                next_frame = GetTickCount64();
                continue;
            }

            dk1_first_level_route_step(
                &route, app.frontend.player_preview.motion.world_x);
            app.route_completed = route.completed;
            convert_frame(&app);
            InvalidateRect(app.window, NULL, FALSE);
        }

        if (app.route_completed && !completion_reported) {
            completion_reported = true;
            write_direct_test_report(&app, &route, &preflight,
                                     manual_restarts, fall_restarts,
                                     debug_warps);
            MessageBoxA(app.window,
                        "You reached the provisional end of Jungle Hijinxs. "
                        "Press Enter or R to restart, or Esc to exit. A test "
                        "report was written beside the executable.",
                        "Jungle Hijinxs test complete", MB_OK | MB_ICONINFORMATION);
        }

        if (GetTickCount64() - title_time >= 250u) {
            char title[600];
            snprintf(title, sizeof(title),
                     "Jungle Hijinxs direct test - preflight:OK route:%u%% "
                     "checkpoints:%u/4 X:%u restarts:%lu/%lu warps:%lu - "
                     "textures:%s - A/D move | Z jump | U barrel | F2 skip "
                     "10%% | R restart | Esc exit",
                     (unsigned)route.progress_percent,
                     (unsigned)route.checkpoints_reached,
                     (unsigned)app.frontend.player_preview.motion.world_x,
                     (unsigned long)manual_restarts,
                     (unsigned long)fall_restarts,
                     (unsigned long)debug_warps,
                     app.startup_visuals_warmed ? "preloaded" : "runtime");
            SetWindowTextA(app.window, title);
            title_time = GetTickCount64();
        }

        next_frame += 16u;
        if (next_frame > GetTickCount64())
            Sleep((DWORD)(next_frame - GetTickCount64()));
        else
            next_frame = GetTickCount64();
    }
    write_direct_test_report(&app, &route, &preflight,
                             manual_restarts, fall_restarts,
                             debug_warps);
    result = 0;

cleanup:
    if (app.window != NULL && IsWindow(app.window))
        DestroyWindow(app.window);
    cleanup_game(&app);
    g_app = NULL;
    return result;
}
