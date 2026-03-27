#ifndef WORLD_H
#define WORLD_H

typedef struct world_nodes world_nodes;
struct world_nodes
{
    mat4* transforms;
    i32** children;
    u32*  children_count;
    i32*  parents;
    i32*  mesh_ids;
    i32   count;
};

typedef struct world world;
struct world
{
    cpu_arena*  arena;
    world_nodes nodes;
};

global world g_world = {0};

void world_init();
void world_load_model(const c8* file);

mat4 world_node_transform(i32 id, coord_space space);

#endif // WORLD_H
