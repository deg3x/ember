#ifndef QUATERNION_H
#define QUATERNION_H

#pragma warning(push)
#pragma warning(disable: 4201)

typedef union quat_t quat_t;
union quat_t
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    f32 data[4];
};

#pragma warning(pop)

#define QUAT_IDENTITY (quat_t){ 0.0f,  0.0f,  0.0f, 1.0f}

internal quat_t quat_norm(quat_t* q);
internal quat_t quat_renorm(quat_t* q);

internal quat_t quat_add(quat_t* lhs, quat_t* rhs);
internal quat_t quat_sub(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul_s(quat_t* lhs, f32 rhs);

internal quat_t quat_inv(quat_t* q);
internal quat_t quat_conj(quat_t* q);

internal f32 quat_dot(quat_t* lhs, quat_t* rhs);
internal f32 quat_len(quat_t* q);
internal f32 quat_len_sqr(quat_t* q);

#endif
