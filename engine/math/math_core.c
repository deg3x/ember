b32 math_approx_zero(f32 value)
{
    return (value > -MATH_FLT_MIN) && (value < MATH_FLT_MIN);
}

b32 math_approx_equal(f32 a, f32 b, f32 error)
{
    return ((a + error) > b) && ((a - error) < b);
}

f32 math_sign(f32 value)
{
    if (value < 0.0f)
    {
        return -1.0f;
    }

    if (value > 0.0f)
    {
        return 1.0f;
    }

    return 0.0f;
}

f32 math_angle_wrap(f32 angle)
{
    f32 result = math_mod(angle, MATH_TWO_PI);

    if (result < 0.0f)
    {
        return result + MATH_TWO_PI;
    }

    return result;
}

f32 math_saturate(f32 value)
{
    return CLAMP(value, 0.0f, 1.0f);
}

f32 math_fast_sin(f32 angle)
{
    return math_fast_cos(angle - MATH_HALF_PI);
}

f32 math_fast_cos(f32 angle)
{
    f32 angle_wrapped = math_angle_wrap(angle);

    if (angle_wrapped < MATH_HALF_PI)
    {
        return math_fast_cos_zero_to_half_pi(angle_wrapped);
    }

    if (angle_wrapped < MATH_PI)
    {
        return -math_fast_cos_zero_to_half_pi(MATH_PI - angle_wrapped);
    }

    if (angle_wrapped < MATH_THREE_PI_HALF)
    {
        return -math_fast_cos_zero_to_half_pi(angle_wrapped - MATH_PI);
    }

    return math_fast_cos_zero_to_half_pi(MATH_TWO_PI - angle_wrapped);
}

f32 math_fast_cos_zero_to_half_pi(f32 angle)
{
    f32 a0 = 1.0f;
    f32 a2 = 2.0f * MATH_PI_INV - 12.0f * MATH_PI_SQR_INV;
    f32 a3 = 16.0f * MATH_PI_CUBE_INV - 4.0f * MATH_PI_SQR_INV;

    f32 angle_sqr = angle * angle;

    return a0 + a2 * angle_sqr + a3 * angle_sqr * angle;
}

f32 math_fast_inv_sqrt_approx_one(f32 value)
{
    f32 a0 =  1.875f;
    f32 a1 = -1.250f;
    f32 a2 =  0.375f;

    return a0 + a1 * value + a2 * value * value;
}
