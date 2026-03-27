typedef u32 coord_space;
enum
{
    COORD_SPACE_local,
    COORD_SPACE_world
};

typedef struct vertex vertex;
struct vertex
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
};

typedef struct mesh mesh;
struct mesh
{
    vertex* vertices;
    u32*    indices;
    u32     vertex_count;
    u32     index_count;
};
