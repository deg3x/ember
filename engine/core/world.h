#ifndef WORLD_H
#define WORLD_H

#define WORLD_MEM_SIZE          MB(128)
#define WORLD_COUNT_TRANSFORMS (MB(112) / sizeof(mat4))
#define WORLD_COUNT_PARENTS    (MB(16)  / sizeof(i32))

typedef struct world_nodes world_nodes;
struct world_nodes
{
    mat4* transforms;
    i32*  parents;
    i32   count;
};

typedef struct world world;
struct world
{
    cpu_arena*  arena;
    world_nodes nodes;
};

internal world world_init();
internal void  world_load_model(world* w, const c8* file);
internal mat4  world_node_transform(world* w, i32 id, coord_space space);

#endif // WORLD_H
