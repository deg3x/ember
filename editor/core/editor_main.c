inline void program_main()
{
    editor_main();
}

inline void editor_main()
{
    platform_hnd_t window_handle = platform_gfx_wnd_create("Ember Engine");

    renderer_init(window_handle);

    world_t world = world_init();

    platform_gfx_wnd_show(window_handle);

    world_entity_t cube = world_entity_create(&world, &g_mesh_cube, 1, &MAT4_IDENTITY);
    (void)cube;

    for(;;)
    {
        platform_gfx_process_events();
        platform_timer_update(&g_program_state.timer);

        // f32_t delta_time = (f32_t)platform_timer_delta(&g_program_state.timer);

        if (!g_program_state.is_running)
        {
            break;
        }

        renderer_update(window_handle);
    }

    renderer_destroy();
}
