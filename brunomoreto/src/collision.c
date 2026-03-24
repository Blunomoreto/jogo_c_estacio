#include "collision.h"

float vec2_length_sq(Vec2 v) {
    return v.x * v.x + v.y * v.y;
}

float vec2_distance_sq(Vec2 a, Vec2 b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

int circle_vs_circle(Vec2 a, float ra, Vec2 b, float rb) {
    const float r = ra + rb;
    return vec2_distance_sq(a, b) <= r * r;
}

int circle_vs_aabb(Vec2 c, float r, Vec2 bmin, Vec2 bmax) {
    float closestX = c.x;
    float closestY = c.y;
    float dx;
    float dy;

    if (closestX < bmin.x) closestX = bmin.x;
    if (closestX > bmax.x) closestX = bmax.x;
    if (closestY < bmin.y) closestY = bmin.y;
    if (closestY > bmax.y) closestY = bmax.y;

    dx = c.x - closestX;
    dy = c.y - closestY;
    return (dx * dx + dy * dy) <= r * r;
}
