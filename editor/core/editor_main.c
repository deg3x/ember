inline void program_main()
{
    editor_main();
}

inline void editor_main()
{
    platform_hnd_t window_handle = platform_gfx_wnd_create("Ember Engine");

    renderer_init(window_handle);

    world_t world = world_init();
    (void)world;

    platform_gfx_wnd_show(window_handle);

    renderer_node_t node = {
        0,
        1
    };

    renderer_ssbo_t ssbo = {
        MAT4_IDENTITY
    };

    renderer_create_nodes(&node, &ssbo, 1);
    renderer_create_meshes(&g_mesh_cube, 1);

    memcpy(world.nodes.transforms + world.nodes.count, &MAT4_IDENTITY, sizeof(mat4_t));

    world.nodes.parents[0] = -1;
    world.nodes.count     += 1;

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
