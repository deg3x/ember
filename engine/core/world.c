internal world_t world_init()
{
    world_t result = {0};

    cpu_arena_params_t params = { WORLD_MEM_SIZE, WORLD_MEM_SIZE, 0 };

    result.arena            = cpu_arena_init(&params);
    result.nodes.transforms = MEMORY_PUSH_ZERO(result.arena, mat4_t, WORLD_COUNT_TRANSFORMS);
    result.nodes.parents    = MEMORY_PUSH_ZERO(result.arena, i32, WORLD_COUNT_PARENTS);

    return result;
}

internal world_entity_t world_load_model(world_t* world, const c8* file)
{
    EMBER_TRACE_BEGIN(gltf_parse);

    gltf_data_t gltf = gltf_parse_file(file);

    EMBER_TRACE_END(gltf_parse);

    world_entity_t ret = { -1 };

    memcpy(world->nodes.transforms + world->nodes.count, gltf.transforms, gltf.node_count * sizeof(mat4_t));

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        i32 node_id = world->nodes.count + i;
        i32 parent;

        if (gltf.parents[i] < 0)
        {
            parent = gltf.parents[i];

            // TODO(KB): We may need to find a way to handle models with multiple root nodes
            // EMBER_ASSERT(ret.id == -1);

            ret.id = node_id;
        }
        else
        {
            parent = gltf.parents[i] + world->nodes.count;
        }

        world->nodes.parents[node_id] = parent;
    }

    for (i32 i = 0; i < gltf.node_count; i++)
    {
        i32 node_id = world->nodes.count + i;
        i32 mesh_id = gltf.mesh_ids[i];

        if (mesh_id < 0)
        {
            continue;
        }

        renderer_ssbo_t ssbo = {
            world_node_transform(world, node_id, COORD_SPACE_world)
        };

        renderer_node_t node = {
            gltf.mesh_offsets[mesh_id] + g_renderer.mesh_count,
            gltf.mesh_primitives[mesh_id]
        };

        renderer_create_nodes(&node, &ssbo, 1);
    }

    renderer_create_meshes(gltf.meshes, gltf.primitive_count);

    world->nodes.count += gltf.node_count;

    gltf_free(&gltf);

    EMBER_ASSERT(ret.id >= 0);

    return ret;
}

internal world_entity_t world_entity_create(world_t* world, mesh_t* meshes, i32 mesh_count, mat4_t* transform)
{
    mat4_t trs = transform != NULL ? *transform : MAT4_IDENTITY;

    if (meshes != NULL && mesh_count > 0)
    {
        renderer_node_t node = {
            g_renderer.mesh_count,
            mesh_count
        };

        renderer_ssbo_t ssbo = {
            trs
        };

        renderer_create_nodes(&node, &ssbo, 1);
        renderer_create_meshes(meshes, mesh_count);
    }

    memcpy(world->nodes.transforms + world->nodes.count, &trs, sizeof(mat4_t));

    world_entity_t ret = { world->nodes.count };

    world->nodes.parents[world->nodes.count] = -1;
    world->nodes.count                      +=  1;

    return ret;
}

internal mat4_t world_node_transform(world_t* world, i32 id, coord_space_t space)
{
    mat4_t result = world->nodes.transforms[id];

    if (space == COORD_SPACE_local)
    {
        return result;
    }

    i32 current = id;
    i32 parent  = world->nodes.parents[id]; 

    while (parent >= 0)
    {
        current = parent;
        parent  = world->nodes.parents[current];
        result  = mat4_mul(&world->nodes.transforms[current], &result);
    }

    return result;
}
