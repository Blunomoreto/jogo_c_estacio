#include "collision.h"
#include "maths.h"

int collision_circle_vs_circle(Vec2 a, float ra, Vec2 b, float rb)
{
    const float r = ra + rb;
    return maths_vec2_distance_sq(a, b) <= r * r;
}

int collision_circle_vs_aabb(Vec2 c, float r, Vec2 bmin, Vec2 bmax)
{
    float closestX = c.x;
    float closestY = c.y;
    float dx;
    float dy;

    if (closestX < bmin.x)
        closestX = bmin.x;
    if (closestX > bmax.x)
        closestX = bmax.x;
    if (closestY < bmin.y)
        closestY = bmin.y;
    if (closestY > bmax.y)
        closestY = bmax.y;

    dx = c.x - closestX;
    dy = c.y - closestY;
    return (dx * dx + dy * dy) <= r * r;
}

int collision_inside_rect(float x, float y, float rx, float ry, float rw, float rh)
{
    return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}
