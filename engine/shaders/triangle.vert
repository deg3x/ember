#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
}ubo;

layout(std430, binding = 1) buffer Transforms
{
    mat4 model[];
}transforms;

layout(std430, binding = 2) buffer DrawData
{
    uint transform_id[];
}draw_data;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec3 frag_normal;

void main()
{
    uint model_id = draw_data.transform_id[gl_InstanceIndex];
    mat4 model    = transforms.model[model_id];

    gl_Position = ubo.proj * ubo.view * model * vec4(position, 1.0);

    frag_color  = color;
    frag_normal = normal;
}
