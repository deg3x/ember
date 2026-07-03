internal world_t world_init()
{
    world_t result = {0};

    cpu_arena_params_t params = { WORLD_MEM_SIZE, WORLD_MEM_SIZE, 0 };

    result.arena              = cpu_arena_init(&params);
    result.nodes.transforms   = MEMORY_PUSH_ZERO(result.arena, mat4_t, WORLD_NODE_COUNT);
    result.nodes.renderer_ids = MEMORY_PUSH_ZERO(result.arena, i32, WORLD_NODE_COUNT);
    result.nodes.parents      = MEMORY_PUSH_ZERO(result.arena, i32, WORLD_NODE_COUNT);

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
            world->nodes.renderer_ids[node_id] = -1;

            continue;
        }

        renderer_ssbo_t ssbo = {
            world_node_model(world, node_id, COORD_SPACE_world)
        };

        renderer_node_t node = {
            gltf.mesh_offsets[mesh_id] + g_renderer.mesh_count,
            gltf.mesh_primitives[mesh_id]
        };

        world->nodes.renderer_ids[node_id] = renderer_create_nodes(&node, &ssbo, 1);
    }

    for (i32 i = 0; i < gltf.material_count; i++)
    {
        material_t* mat = &gltf.materials[i];

        mat->tex_id_al += (mat->tex_id_al >= 0) ? g_renderer.tex_count : 0;
        mat->tex_id_mr += (mat->tex_id_mr >= 0) ? g_renderer.tex_count : 0;
        mat->tex_id_nm += (mat->tex_id_nm >= 0) ? g_renderer.tex_count : 0;
        mat->tex_id_ao += (mat->tex_id_ao >= 0) ? g_renderer.tex_count : 0;
        mat->tex_id_em += (mat->tex_id_em >= 0) ? g_renderer.tex_count : 0;

        mat->tex_id_al = CLAMP(mat->tex_id_al, 0, RENDERER_TEX_COUNT_MAX);
        mat->tex_id_mr = CLAMP(mat->tex_id_mr, 0, RENDERER_TEX_COUNT_MAX);
        mat->tex_id_nm = CLAMP(mat->tex_id_nm, 0, RENDERER_TEX_COUNT_MAX);
        mat->tex_id_ao = CLAMP(mat->tex_id_ao, 0, RENDERER_TEX_COUNT_MAX);      
        mat->tex_id_em = CLAMP(mat->tex_id_em, 0, RENDERER_TEX_COUNT_MAX);

        // NOTE(KB): Hitting these asserts means its time to parse extra UV channels from the gltf
        EMBER_ASSERT(mat->tex_uv_al <= 1);
        EMBER_ASSERT(mat->tex_uv_mr <= 1);
        EMBER_ASSERT(mat->tex_uv_nm <= 1);
        EMBER_ASSERT(mat->tex_uv_ao <= 1);
        EMBER_ASSERT(mat->tex_uv_em <= 1);
    }

    for (i32 i = 0; i < gltf.primitive_count; i++)
    {
        mesh_t* mesh = &gltf.meshes[i];

        if (mesh->material_id >= 0)
        {
            mesh->material_id += g_renderer.mat_count;
        }

        mesh->material_id = CLAMP(mesh->material_id, 0, RENDERER_MAT_COUNT_MAX);

        if (!gltf.mesh_has_tangents[i])
        {
            mesh_generate_tangents(mesh);
        }
    }

    for (i32 i = 0; i < gltf.texture_count; i++)
    {
        i32 width;
        i32 height;
        i32 channels;

        stbi_uc* pixels = stbi_load_from_memory(
            gltf.textures[i].data,
            gltf.textures[i].size,
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );

        renderer_img_format_t format = gltf.textures[i].type == TEXTURE_TYPE_NORMAL
                                     ? RENDERER_IMG_FORMAT_rgba_unorm
                                     : RENDERER_IMG_FORMAT_rgba_srgb;

        renderer_tex_info_t tex_info = {
            width,
            height,
            (u8*)pixels,
            width * height * 4,
            format,
            RENDERER_IMG_TILING_optimal,
            RENDERER_IMG_USAGE_FLAGS_transfer_dst | RENDERER_IMG_USAGE_FLAGS_sampled
        };

        renderer_create_textures(&tex_info, 1);

        stbi_image_free(pixels);
    }

    renderer_create_materials(gltf.materials, gltf.material_count);
    renderer_create_meshes(gltf.meshes, gltf.primitive_count);

    world->nodes.count += gltf.node_count;

    gltf_free(&gltf);

    EMBER_ASSERT(ret.id >= 0);

    return ret;
}

internal world_entity_t world_entity_create(world_t* world, mesh_t* meshes, i32 mesh_count, mat4_t* transform)
{
    i32 node_id = world->nodes.count;
    mat4_t trs  = transform != NULL ? *transform : MAT4_IDENTITY;

    if (meshes != NULL && mesh_count > 0)
    {
        renderer_node_t node = {
            g_renderer.mesh_count,
            mesh_count
        };

        renderer_ssbo_t ssbo = {
            trs
        };

        world->nodes.renderer_ids[node_id] = renderer_create_nodes(&node, &ssbo, 1);
        renderer_create_meshes(meshes, mesh_count);
    }
    else
    {
        world->nodes.renderer_ids[node_id] = -1;
    }

    memcpy(world->nodes.transforms + world->nodes.count, &trs, sizeof(mat4_t));

    world_entity_t ret = { world->nodes.count };

    world->nodes.parents[world->nodes.count] = -1;
    world->nodes.count                      +=  1;

    return ret;
}

internal mat4_t world_node_model(world_t* world, i32 node_id, coord_space_t space)
{
    mat4_t result = world->nodes.transforms[node_id];

    if (space == COORD_SPACE_local)
    {
        return result;
    }

    i32 current = node_id;
    i32 parent  = world->nodes.parents[node_id]; 

    while (parent >= 0)
    {
        current = parent;
        parent  = world->nodes.parents[current];
        result  = mat4_mul(&world->nodes.transforms[current], &result);
    }

    return result;
}

internal void world_entity_set_transform(world_t* world, world_entity_t entity, transform_t* transform)
{
    mat4_t model = mat4_model(&transform->position, &transform->rotation, &transform->scale);

    world->nodes.transforms[entity.id] = model;

    for (i32 i = 0; i < world->nodes.count; i++)
    {
        i32 current = -1;
        i32 parent  = i;
        b32 update  = EMBER_FALSE;

        while (parent >= 0)
        {
            if (parent == entity.id)
            {
                update = EMBER_TRUE;

                break;
            }

            current = parent;
            parent  = world->nodes.parents[current];
        }

        if (update)
        {
            model = world_node_model(world, i, COORD_SPACE_world);

            i32 rnd_id = world->nodes.renderer_ids[i];
            if (rnd_id >= 0)
            {
                renderer_update_trs(rnd_id, &model, 1);
            }
        }
    }
}
