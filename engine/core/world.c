world world_init()
{
    world result = {0};

    cpu_arena_params params = { WORLD_MEM_SIZE, WORLD_MEM_SIZE, 0 };

    result.arena            = cpu_arena_init(&params);
    result.nodes.transforms = MEMORY_PUSH_ZERO(result.arena, mat4, WORLD_COUNT_TRANSFORMS);
    result.nodes.parents    = MEMORY_PUSH_ZERO(result.arena, i32, WORLD_COUNT_PARENTS);

    return result;
}

void world_load_model(world* w, const c8* file)
{
    EMBER_TRACE_BEGIN(gltf_parse);

    gltf_data gltf = gltf_parse_file(file);

    EMBER_TRACE_END(gltf_parse);

    memcpy(w->nodes.transforms + w->nodes.count, gltf.transforms, gltf.node_count * sizeof(mat4));

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        i32 node_id = w->nodes.count + i;
        i32 parent  = gltf.parents[i] < 0 ? gltf.parents[i] : gltf.parents[i] + w->nodes.count;

        w->nodes.parents[node_id] = parent;
    }

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        i32 node_id = w->nodes.count + i;
        i32 mesh_id = gltf.mesh_ids[i];

        if (mesh_id < 0)
        {
            continue;
        }

        renderer_ssbo ssbo = {
            world_node_transform(w, node_id, COORD_SPACE_world)
        };

        renderer_node node = {
            gltf.mesh_offsets[mesh_id] + g_renderer.mesh_count,
            gltf.mesh_primitives[mesh_id]
        };

        renderer_create_nodes(&node, &ssbo, 1);
    }

    renderer_create_meshes(gltf.meshes, gltf.primitive_count);

    w->nodes.count += gltf.node_count;

    gltf_free(&gltf);
}

mat4 world_node_transform(world* w, i32 id, coord_space space)
{
    mat4 result = w->nodes.transforms[id];

    if (space == COORD_SPACE_local)
    {
        return result;
    }

    i32 current = id;
    i32 parent  = w->nodes.parents[id]; 

    while (parent >= 0)
    {
        current = parent;
        parent  = w->nodes.parents[current];
        result  = mat4_mul(&w->nodes.transforms[current], &result);
    }

    return result;
}
