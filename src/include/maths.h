#ifndef MATHS_H
#define MATHS_H

#include "types.h"

Vec2 maths_vec2(float x, float y);
Vec2 maths_vec2_add(Vec2 a, Vec2 b);
Vec2 maths_vec2_sub(Vec2 a, Vec2 b);
Vec2 maths_vec2_mul(Vec2 v, float s);
Vec2 maths_vec2_norm(Vec2 v);

float maths_randf(float minV, float maxV);
float maths_vec2_len(Vec2 v);
float maths_vec2_length_sq(Vec2 v);
float maths_vec2_distance_sq(Vec2 a, Vec2 b);
float maths_clamp_min_max(float a, float min, float max);
float maths_clamp_min(float a, float min);
float maths_clamp_max(float a, float max);

#endif
