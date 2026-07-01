#version 450

struct draw_data_t
{
    int transform_id;
    int material_id;
};

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
}ubo;

layout(std430, set = 0, binding = 1) readonly buffer TransformBuffer
{
    mat4 transforms[];
};

layout(std430, set = 0, binding = 2) readonly buffer DrawDataBuffer
{
    draw_data_t draw_info[];
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec3 color;
layout(location = 4) in vec2 uv0;
layout(location = 5) in vec2 uv1;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out mat3 frag_tbn;
layout(location = 4) out vec2 frag_uvs[2];
layout(location = 6) flat out uint instance_id;

void main()
{
    int model_id = draw_info[gl_InstanceIndex].transform_id;
    mat4 model   = transforms[model_id];

    gl_Position = ubo.proj * ubo.view * model * vec4(position, 1.0);

    mat4 model_adj = transpose(determinant(model) * inverse(model));

    vec3 n = normalize(model_adj * vec4(normal, 1.0)).xyz;
    vec3 t = (model * tangent).xyz;
    t      = normalize(t - n * dot(t, n));
    vec3 b = normalize(cross(n, t) * tangent.w);

    frag_color  = color;
    frag_tbn    = mat3(t, b, n);
    frag_uvs[0] = uv0;
    frag_uvs[1] = uv1;
    instance_id = gl_InstanceIndex;
}
