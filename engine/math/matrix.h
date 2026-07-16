#ifndef MATRIX_H
#define MATRIX_H

#pragma warning(push)
#pragma warning(disable: 4201)

// NOTE(KB): Matrices are stored in a column-first order
typedef union mat4_t mat4_t;
union mat4_t
{
    struct
    {
        vec4_t cols[4];
    };

    f32 m[4][4];
};

#pragma warning(pop)

#define MAT4_IDENTITY (mat4_t)  \
    {                           \
        1.0f, 0.0f, 0.0f, 0.0f, \
        0.0f, 1.0f, 0.0f, 0.0f, \
        0.0f, 0.0f, 1.0f, 0.0f, \
        0.0f, 0.0f, 0.0f, 1.0f  \
    }

internal f32 mat4_determinant(mat4_t* matrix);

internal mat4_t mat4_add(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_sub(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul_s(mat4_t* lhs, f32 rhs);

internal mat4_t mat4_inverse(mat4_t* matrix);
internal mat4_t mat4_transpose(mat4_t* matrix);

#endif // MATRIX_H
