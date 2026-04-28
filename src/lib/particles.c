#include "particles.h"
#include "maths.h"
#include "render.h"

#include <math.h>

void particles_spawn(Game *g, Vec2 p, int count, Color color)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        int j;
        for (j = 0; j < MAX_PARTICLES; ++j)
        {
            Particle *pt = &g->particles[j];
            if (!pt->active)
            {
                float ang = maths_randf(0.0f, 2.0f * (float)M_PI);
                float spd = maths_randf(30.0f, 180.0f);
                pt->active = 1;
                pt->pos = p;
                pt->vel = maths_vec2(cosf(ang) * spd, sinf(ang) * spd);
                pt->size = maths_randf(2.0f, 5.0f);
                pt->life = maths_randf(0.2f, 0.6f);
                pt->color = color;
                break;
            }
        }
    }
}

void particles_draw(Game *g)
{
    int i;
    for (i = 0; i < MAX_PARTICLES; ++i)
    {
        Particle *pt = &g->particles[i];
        if (!pt->active)
        {
            continue;
        }
        render_circle(pt->pos, pt->size, (Color){pt->color.r, pt->color.g, pt->color.b, pt->color.a * (pt->life * 2.0f)}, 8);
    }
}
