#version 450

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 world_normal;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 light_dir    = normalize(vec3(-0.25, -1.0, 0.45));
    float light_alpha = clamp(dot(normalize(world_normal), -light_dir), 0.0, 1.0);

    out_color = vec4(frag_color * light_alpha, 1.0);
}
