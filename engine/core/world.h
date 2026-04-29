#ifndef WORLD_H
#define WORLD_H

#define WORLD_MEM_SIZE          MB(128)
#define WORLD_COUNT_TRANSFORMS (MB(112) / sizeof(mat4_t))
#define WORLD_COUNT_PARENTS    (MB(16)  / sizeof(i32))

typedef struct world_entity_t world_entity_t;
struct world_entity_t
{
    i32 id;
};

typedef struct world_nodes_t world_nodes_t;
struct world_nodes_t
{
    mat4_t* transforms;
    i32*    parents;
    i32     count;
};

typedef struct world_t world_t;
struct world_t
{
    cpu_arena_t*  arena;
    world_nodes_t nodes;
};

internal world_t        world_init();
internal world_entity_t world_load_model(world_t* world, const c8* file);
internal world_entity_t world_entity_create(world_t* world, mesh_t* meshes, i32 mesh_count, mat4_t* transform);
internal mat4_t         world_node_transform(world_t* world, i32 id, coord_space_t space);

#endif // WORLD_H
