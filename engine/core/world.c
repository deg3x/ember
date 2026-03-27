void world_init()
{
    cpu_arena_params params = { GB(1), GB(1), 0 };

    g_world.arena = cpu_arena_init(&params);
}

void world_load_model(const c8* file)
{
    EMBER_TRACE_BEGIN(gltf_parse);

    gltf_data gltf = gltf_parse_file(file, g_world.arena);

    EMBER_TRACE_END(gltf_parse);

    renderer_create_meshes(gltf.meshes, gltf.mesh_count);

    g_world.nodes.transforms     = gltf.transforms;
    g_world.nodes.children       = gltf.children;
    g_world.nodes.children_count = gltf.children_count;
    g_world.nodes.parents        = gltf.parents;
    g_world.nodes.mesh_ids       = gltf.mesh_ids;

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        if (gltf.mesh_ids[i] < 0)
        {
            continue;
        }

        mat4 transform  = world_node_transform(i, COORD_SPACE_world);

        // NOTE(KB): This breaks if we have multiple instances that use the same mesh
        //           Improve by introducing instance IDs (also in renderer)
        for (i32 j = 0; j < RENDERER_FRAMES_IN_FLIGHT; j++)
        {
            u8* dest = (u8*)g_renderer.buffers.ssbo_mapped[j] + gltf.mesh_ids[i] * sizeof(mat4);

            memcpy(dest, &transform, sizeof(mat4));
        }
    }

    g_world.nodes.count += gltf.node_count;
}

mat4 world_node_transform(i32 id, coord_space space)
{
    mat4 result = g_world.nodes.transforms[id];

    if (space == COORD_SPACE_local)
    {
        return result;
    }

    i32 current = id;
    i32 parent  = g_world.nodes.parents[id]; 

    while (parent >= 0)
    {
        current = parent;
        parent  = g_world.nodes.parents[current];
        result  = mat4_mul(&g_world.nodes.transforms[current], &result);
    }

    return result;
}
