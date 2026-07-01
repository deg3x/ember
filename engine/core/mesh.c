internal void mesh_generate_tangents(mesh_t* mesh)
{
    EMBER_ASSERT(mesh != NULL);

    vertex_t* vertices = mesh->vertices;
    i32* indices       = mesh->indices;

    for (i32 i = 0; i < mesh->index_count; i += 3)
    {
        vertex_t* v0 = &vertices[indices[i]];
        vertex_t* v1 = &vertices[indices[i + 1]];
        vertex_t* v2 = &vertices[indices[i + 2]];

        vec3_t dpos0 = vec3_sub(&v1->position, &v0->position);
        vec3_t dpos1 = vec3_sub(&v2->position, &v0->position);
        vec2_t duv0  = vec2_sub(&v1->uv0, &v0->uv0);
        vec2_t duv1  = vec2_sub(&v2->uv0, &v0->uv0);

        f32 inv_det = 1.0f / (duv0.x * dpos1.y - duv0.y * dpos1.x);

        vec3_t x = vec3_mul_s(&dpos0, duv1.y);
        vec3_t y = vec3_mul_s(&dpos1, duv0.y);

        vec3_t xy = vec3_sub(&x, &y);

        vec3_t tangent = vec3_mul_s(&xy, inv_det);
        vec4_t result  = (vec4_t){tangent.x, tangent.y, tangent.z, 1.0f};

        v0->tangent = result;
        v1->tangent = result;
        v2->tangent = result;
    }
}
