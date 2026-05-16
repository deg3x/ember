#ifndef TRANSFORM_H
#define TRANSFORM_H

typedef struct transform_t transform_t;
struct transform_t
{
    vec3_t position;
    quat_t rotation;
    vec3_t scale;
};

internal transform_t transform_from_mat4(mat4_t* matrix);

internal vec3_t transform_fw(transform_t* transform);
internal vec3_t transform_bk(transform_t* transform);
internal vec3_t transform_up(transform_t* transform);
internal vec3_t transform_dn(transform_t* transform);
internal vec3_t transform_rt(transform_t* transform);
internal vec3_t transform_lt(transform_t* transform);

#endif // TRANSFORM_H
