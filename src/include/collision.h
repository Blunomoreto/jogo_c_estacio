#ifndef COLLISION_H
#define COLLISION_H

#include "types.h"

float vec2_length_sq(Vec2 v);
float vec2_distance_sq(Vec2 a, Vec2 b);
int circle_vs_circle(Vec2 a, float ra, Vec2 b, float rb);
int circle_vs_aabb(Vec2 c, float r, Vec2 bmin, Vec2 bmax);

#endif
