inline void program_main()
{
    editor_main();
}

inline void editor_main()
{
    g_program_state.is_running = EMBER_TRUE;
    g_program_state.timer      = platform_timer_init();

    platform_init();

    platform_hnd_t window_handle = platform_gfx_wnd_create("Ember Engine");

    renderer_init(window_handle);

    world_t world = world_init();
    (void)world;

    platform_gfx_wnd_show(window_handle);

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
