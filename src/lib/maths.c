#include "maths.h"

#include <math.h>
#include <stdlib.h>

float maths_randf(float minV, float maxV)
{
    return minV + (maxV - minV) * ((float)rand() / (float)RAND_MAX);
}

Vec2 maths_vec2(float x, float y)
{
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

Vec2 maths_vec2_add(Vec2 a, Vec2 b)
{
    return maths_vec2(a.x + b.x, a.y + b.y);
}

Vec2 maths_vec2_sub(Vec2 a, Vec2 b)
{
    return maths_vec2(a.x - b.x, a.y - b.y);
}

Vec2 maths_vec2_mul(Vec2 v, float s)
{
    return maths_vec2(v.x * s, v.y * s);
}

float maths_vec2_len(Vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vec2 maths_vec2_norm(Vec2 v)
{
    float len = maths_vec2_len(v);
    if (len <= 0.0001f)
    {
        return maths_vec2(0.0f, 1.0f);
    }
    return maths_vec2(v.x / len, v.y / len);
}

float maths_vec2_length_sq(Vec2 v)
{
    return v.x * v.x + v.y * v.y;
}

float maths_vec2_distance_sq(Vec2 a, Vec2 b)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

float maths_clamp_min_max(float a, float min, float max)
{
    const float clamped = a < min ? min : a;
    return clamped > max ? max : clamped;
}

float maths_clamp_min(float a, float min)
{
    const float clamped = a < min ? min : a;
    return clamped;
}

float maths_clamp_max(float a, float max)
{
    const float clamped = a > max ? max : a;
    return clamped;
}
