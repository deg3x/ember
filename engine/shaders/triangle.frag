#version 450

#extension GL_EXT_nonuniform_qualifier : enable

struct DrawData
{
    int transform_id;
    int material_id;
};

struct Material
{
    vec4  color;
    vec3  color_em;

    float metal;
    float rough;
    float ao;
    int   alpha_mode;
    float alpha_cutoff;
    float normal_scale;
    int   double_sided;

    int   tex_id_al;
    int   tex_id_mr;
    int   tex_id_nm;
    int   tex_id_ao;
    int   tex_id_em;

    int   tex_uv_al;
    int   tex_uv_mr;
    int   tex_uv_nm;
    int   tex_uv_ao;
    int   tex_uv_em;
};

layout(std430, set = 0, binding = 2) readonly buffer DrawDataBuffer
{
    DrawData draw_info[];
};

layout(std430, set = 0, binding = 3) readonly buffer MaterialBuffer
{
    Material materials[];
};

layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec2 frag_uv;
layout(location = 3) flat in uint instance_id;

layout(location = 0) out vec4 out_color;

void main()
{
    int id_mat    = draw_info[instance_id].material_id;
    int id_albedo = materials[id_mat].tex_id_al;

    vec4 tex_al       = texture(textures[nonuniformEXT(id_albedo)], frag_uv);
    vec3 light_dir    = normalize(vec3(-0.25, -1.0, -0.45));
    float light_alpha = clamp(dot(normalize(world_normal), -light_dir), 0.0, 1.0);

    out_color = vec4(tex_al.rgb * frag_color * light_alpha, 1.0);
}
