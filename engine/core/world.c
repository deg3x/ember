void world_init()
{
    cpu_arena_params params = { GB(4), GB(4), 0 };

    g_world.arena = cpu_arena_init(&params);
}

void world_load_model(const c8* file)
{
    EMBER_TRACE_BEGIN(gltf_parse);

    gltf_data gltf = gltf_parse_file(file, g_world.arena);

    EMBER_TRACE_END(gltf_parse);

    renderer_create_meshes(gltf.meshes, gltf.primitive_count);

    g_world.nodes.transforms     = gltf.transforms;
    g_world.nodes.children       = gltf.children;
    g_world.nodes.children_count = gltf.children_count;
    g_world.nodes.parents        = gltf.parents;
    g_world.nodes.mesh_ids       = gltf.mesh_ids;

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        i32 mesh_id = gltf.mesh_ids[i];

        if (mesh_id < 0)
        {
            continue;
        }

        renderer_ssbo ssbo = {
            world_node_transform(i, COORD_SPACE_world)
        };

        renderer_node node = {
            gltf.mesh_offsets[mesh_id],
            gltf.mesh_primitives[mesh_id]
        };

        renderer_create_nodes(&node, &ssbo, 1);
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
