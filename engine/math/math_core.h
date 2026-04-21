#ifndef MATH_CORE_H
#define MATH_CORE_H

#define MATH_PI            3.14159265358f
#define MATH_PI_SQR        9.86960506439f
#define MATH_PI_CUBE       31.0062789916f
#define MATH_PI_INV        0.31830987334f
#define MATH_PI_SQR_INV    0.10132117569f
#define MATH_PI_CUBE_INV   0.03225153312f
#define MATH_TWO_PI        6.28318530717f
#define MATH_HALF_PI       1.57079632679f
#define MATH_THREE_PI_HALF 4.71238899231f
#define MATH_RAD2DEG       57.2957763671f
#define MATH_DEG2RAD       0.01745329238f

#define MATH_EPSILON       1.192092896e-07f

#define MATH_FLT_MIN       1.175494351e-38f
#define MATH_FLT_MAX       3.402823466e+38f

#define POW_2_CHECK(v)         ((v)!=0 && ((v) & ((v) - 1))==0)
#define POW_2_ROUND_UP(v, a)   (((v) + (a) - 1) & (~((a) - 1)))
#define POW_2_ROUND_DOWN(v, a) ((v) & (~((a) - 1)))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define CLAMP_TOP(a, b)    MIN(a, b)
#define CLAMP_BOT(a, b)    MAX(a, b)
#define CLAMP(v, min, max) (((v) < (min)) ? (min) : ((v) > (max) ? (max) : (v)))

///////////////////////////
// FUNCTIONS FROM math.h

#define math_sin(v)      sinf(v)
#define math_cos(v)      cosf(v)
#define math_tan(v)      tanf(v)
#define math_asin(v)     asinf(v)
#define math_acos(v)     acosf(v)
#define math_atan(v)     atanf(v)
#define math_atan2(x, y) atan2f((x), (y))
#define math_mod(x, y)   fmodf((x), (y))
#define math_abs(v)      fabsf(v)
#define math_pow(v, e)   powf((v), (e))
#define math_sqrt(v)     sqrtf(v)
#define math_ceil(v)     ceilf(v)
#define math_floor(v)    floorf(v)

///////////
// TYPES

#pragma warning(push)
#pragma warning(disable: 4201)

typedef union vec2 vec2;
union vec2
{
    struct
    {
        f32 x;
        f32 y;
    };

    struct
    {
        f32 u;
        f32 v;
    };

    f32 data[2];
};

typedef union vec3 vec3;
union vec3
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };

    struct
    {
        f32 u;
        f32 v;
        f32 w;
    };

    f32 data[3];
};

typedef union vec4 vec4;
union vec4
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };

    f32 data[4];
};

typedef union quat quat;
union quat
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

// NOTE(KB): Matrices are stored in a column-first order
typedef union mat4 mat4;
union mat4
{
    struct
    {
        vec4 cols[4];
    };

    f32 m[4][4];
};

#define VEC2_ZERO     (vec2){0.0f, 0.0f}
#define VEC3_ZERO     (vec3){0.0f, 0.0f, 0.0f}
#define VEC3_ONE      (vec3){1.0f, 1.0f, 1.0f}
#define VEC3_UP       (vec3){0.0f, 1.0f, 0.0f}
#define VEC3_RIGHT    (vec3){1.0f, 0.0f, 0.0f}
#define VEC3_FORWARD  (vec3){0.0f, 0.0f, 1.0f}
#define VEC4_ZERO     (vec4){0.0f, 0.0f, 0.0f, 0.0f}
#define QUAT_IDENTITY (quat){0.0f, 0.0f, 0.0f, 1.0f}
#define MAT4_IDENTITY (mat4)  \
    {                           \
        1.0f, 0.0f, 0.0f, 0.0f, \
        0.0f, 1.0f, 0.0f, 0.0f, \
        0.0f, 0.0f, 1.0f, 0.0f, \
        0.0f, 0.0f, 0.0f, 1.0f  \
    }

#pragma warning(pop)

///////////////
// FUNCTIONS

internal b32 math_approx_zero(f32 value);
internal b32 math_approx_equal(f32 a, f32 b, f32 error);

internal f32 math_sign(f32 value);
internal f32 math_angle_wrap(f32 angle);
internal f32 math_saturate(f32 value);
internal f32 math_fast_sin(f32 angle);
internal f32 math_fast_cos(f32 angle);
internal f32 math_fast_cos_zero_to_half_pi(f32 angle);
internal f32 math_fast_inv_sqrt_approx_one(f32 value);

#endif // MATH_CORE_H
