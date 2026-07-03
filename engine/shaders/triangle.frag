#version 450

#extension GL_EXT_nonuniform_qualifier : enable

struct draw_data_t
{
    int transform_id;
    int material_id;
};

struct material_t
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

    int   use_tex_al;
    int   use_tex_mr;
    int   use_tex_nm;
    int   use_tex_ao;
    int   use_tex_em;

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
    draw_data_t draw_info[];
};

layout(std430, set = 0, binding = 3) readonly buffer MaterialBuffer
{
    material_t materials[];
};

layout(set = 1, binding = 0) uniform sampler2D textures[];

layout(location = 0) in vec3 color;
layout(location = 1) in mat3 tbn;
layout(location = 4) in vec2 uvs[2];
layout(location = 6) flat in uint instance_id;

layout(location = 0) out vec4 out_color;

void main()
{
    int id_mat     = draw_info[instance_id].material_id;
    material_t mat = materials[id_mat];

    int id_albedo = mat.tex_id_al;
    int id_normal = mat.tex_id_nm;

    vec2 uvs_al = uvs[mat.tex_uv_al];
    vec2 uvs_mr = uvs[mat.tex_uv_mr];
    vec2 uvs_nm = uvs[mat.tex_uv_nm];
    vec2 uvs_ao = uvs[mat.tex_uv_ao];
    vec2 uvs_em = uvs[mat.tex_uv_em];

    vec4 tex_al = texture(textures[nonuniformEXT(id_albedo)], uvs_al);
    vec3 tex_nm = texture(textures[nonuniformEXT(id_normal)], uvs_nm).rgb * 2.0 - 1.0;
    tex_nm.xy  *= mat.normal_scale;
    tex_nm      = normalize(tex_nm);
    float check = step(0.5, dot(cross(tbn[0], tbn[1]), tbn[2]));
    tex_nm      = normalize((check * tbn + (1 - check) * mat3(-tbn[0], tbn[1], tbn[2])) * tex_nm);

    vec4 albedo = mat.use_tex_al > 0.5 ? tex_al : mat.color;
    vec3 normal = mat.use_tex_nm > 0.5 ? tex_nm : tbn[2];

    if (albedo.a < mat.alpha_cutoff)
        discard;

    vec3 light_dir    = normalize(vec3(-0.25, -1.0, -0.45));
    float light_alpha = clamp(dot(normalize(normal), -light_dir), 0.0, 1.0);

    out_color = vec4(albedo.rgb * color * mat.color.rgb * light_alpha, 1.0f);
    //out_color = vec4(normal, 1.0);
}
