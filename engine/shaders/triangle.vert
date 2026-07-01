#version 450

struct DrawData
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
    DrawData draw_info[];
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_uv;
layout(location = 3) flat out uint instance_id;

void main()
{
    int model_id = draw_info[gl_InstanceIndex].transform_id;
    mat4 model   = transforms[model_id];

    gl_Position = ubo.proj * ubo.view * model * vec4(position, 1.0);

    mat4 model_adj = transpose(determinant(model) * inverse(model));

    frag_color  = color;
    frag_normal = normalize(model_adj * vec4(normal, 1.0)).xyz;
    frag_uv     = uv;
    instance_id = gl_InstanceIndex;
}
