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

    platform_wnd_size_t wnd_size = platform_gfx_wnd_client_get_size(window_handle);

    camera_t camera_data  = {0};
    camera_data.fov       = 60.0f;
    camera_data.aspect    = (f32)wnd_size.width / (f32)wnd_size.height;
    camera_data.clip_near = 0.01f;
    camera_data.clip_far  = 1000.0f;

    transform_t camera_trs;
    camera_trs.position = (vec3_t){0.0f, 1.0f, -3.0f};
    camera_trs.rotation = QUAT_IDENTITY;
    camera_trs.scale    = VEC3_ONE;

    world_entity_t cube   = world_entity_create(&world, &g_mesh_cube, 1, &MAT4_IDENTITY);
    (void)cube;

    for(;;)
    {
        platform_gfx_process_events();
        platform_timer_update(&g_program_state.timer);

        if (!g_program_state.is_running)
        {
            break;
        }

        editor_camera_update(&camera_trs);

        renderer_update_ubo(&camera_trs, &camera_data);
        renderer_update(window_handle);
    }

    renderer_destroy();
}

inline void editor_camera_update(transform_t* camera_trs)
{
    persist f32 camera_speed      = 5.0f;
    persist vec2_t mouse_pos_last = {-1.0f, -1.0f};
    persist vec2_t mouse_pos      = {-1.0f, -1.0f};

    if (!g_input_state.keys[INPUT_KEY_rmb])
    {
        mouse_pos      = (vec2_t){-1.0f, -1.0f};
        mouse_pos_last = (vec2_t){-1.0f, -1.0f};

        return;
    }

    f32 mouse_speed_rot = 5.0f;
    f32 delta_time      = (f32)platform_timer_delta(g_program_state.timer);
    f32 mouse_scroll    = g_input_state.mouse_scroll.y;

    mouse_pos = g_input_state.mouse_pos;
    if (mouse_pos_last.x < 0.0f && mouse_pos_last.y < 0.0f)
    {
        mouse_pos_last = mouse_pos;
    }

    vec2_t mouse_offset = vec2_sub(&mouse_pos, &mouse_pos_last);

    if (!math_approx_zero(mouse_scroll))
    {
        // NOTE(KB): Consume scroll state
        //           We probably want to move this in the platform layer and reset on each update?
        g_input_state.mouse_scroll.y = 0.0f;

        camera_speed += mouse_scroll * 0.005f;
        camera_speed  = MAX(camera_speed, 1.0f);
    }

    /////////////////////////////////////
    // NOTE(KB): CAMERA ROTATION
    if (!math_approx_zero(mouse_offset.x))
    {
        quat_t rot = quat_from_axis_angle(&VEC3_UP, mouse_offset.x * mouse_speed_rot * delta_time);

        camera_trs->rotation = quat_mul(&rot, &camera_trs->rotation);
        camera_trs->rotation = quat_renorm(&camera_trs->rotation);
    }
    if (!math_approx_zero(mouse_offset.y))
    {
        vec3_t cam_rt = transform_rt(camera_trs);
        cam_rt.y = 0.0f;
        cam_rt = vec3_norm(&cam_rt);
        quat_t rot    = quat_from_axis_angle(&cam_rt, mouse_offset.y * mouse_speed_rot * delta_time);

        camera_trs->rotation = quat_mul(&rot, &camera_trs->rotation);
        camera_trs->rotation = quat_renorm(&camera_trs->rotation);
    }

    /////////////////////////////////////
    // NOTE(KB): CAMERA MOVEMENT
    if (g_input_state.keys[INPUT_KEY_w])
    {
        vec3_t camera_fw = transform_fw(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_fw, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }
    if (g_input_state.keys[INPUT_KEY_s])
    {
        vec3_t camera_bk = transform_bk(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_bk, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }
    if (g_input_state.keys[INPUT_KEY_d])
    {
        vec3_t camera_rt = transform_rt(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_rt, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }
    if (g_input_state.keys[INPUT_KEY_a])
    {
        vec3_t camera_lt = transform_lt(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_lt, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }
    if (g_input_state.keys[INPUT_KEY_e])
    {
        vec3_t camera_up = transform_up(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_up, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }
    if (g_input_state.keys[INPUT_KEY_q])
    {
        vec3_t camera_dn = transform_dn(camera_trs);
        vec3_t velocity  = vec3_mul_s(&camera_dn, delta_time * camera_speed);

        camera_trs->position = vec3_add(&camera_trs->position, &velocity);
    }

    if (vec2_len_sqr(&mouse_offset) > MATH_EPSILON)
    {
        mouse_pos_last = mouse_pos;
    }
}
