#ifndef COLLISION_H
#define COLLISION_H

#include "maths.h"

int collision_circle_vs_circle(Vec2 a, float ra, Vec2 b, float rb);
int collision_circle_vs_aabb(Vec2 c, float r, Vec2 bmin, Vec2 bmax);
int collision_inside_rect(float x, float y, float rx, float ry, float rw, float rh);

#endif
