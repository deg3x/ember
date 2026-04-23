#ifndef WORLD_H
#define WORLD_H

#define WORLD_MEM_SIZE          MB(128)
#define WORLD_COUNT_TRANSFORMS (MB(112) / sizeof(mat4_t))
#define WORLD_COUNT_PARENTS    (MB(16)  / sizeof(i32))

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

internal world_t world_init();
internal void    world_load_model(world_t* w, const c8* file);
internal mat4_t  world_node_transform(world_t* w, i32 id, coord_space_t space);

#endif // WORLD_H
