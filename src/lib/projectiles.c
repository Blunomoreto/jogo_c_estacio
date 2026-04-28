#include "projectiles.h"
#include "maths.h"
#include "render.h"

#include <GL/glut.h>
#include <math.h>
#include <string.h>

void projectiles_spawn(Game *g, Vec2 pos, Vec2 dir, int fromPlayer, float speed, float damage, float radius, float life, GuidanceType guidance, int targetIdx, float maxLatAccel)
{
    int i;
    for (i = 0; i < MAX_PROJECTILES; ++i)
    {
        Projectile *p = &g->projectiles[i];
        if (!p->active)
        {
            memset(p, 0, sizeof(*p));
            p->active = 1;
            p->fromPlayer = fromPlayer;
            p->pos = pos;
            p->vel = maths_vec2_mul(dir, speed);
            p->radius = radius;
            p->life = life;
            p->damage = damage;

            p->guidance = guidance;
            p->targetIdx = targetIdx;
            p->actualLatAccel = 0.0f;
            p->fuelTimer = (guidance != GUIDANCE_NONE) ? GUIDED_FUEL_TIME : 0.0f;
            p->prevDist = 99999.0f;
            p->missed = 0;
            p->sdTimer = 0.0f;
            p->maxLatAccel = maxLatAccel;
            return;
        }
    }
}

void projectiles_draw(Game *g)
{
    int i;

    for (i = 0; i < MAX_PROJECTILES; ++i)
    {
        Projectile *p = &g->projectiles[i];
        if (!p->active)
        {
            continue;
        }

        if (p->guidance != GUIDANCE_NONE)
        {
            float angle = atan2f(p->vel.y, p->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Color c;
            if (p->missed)
            {
                c = (Color){1.0f, 0.0f, 0.0f, 1.0f};
            }
            else if (p->guidance == GUIDANCE_APNG)
            {
                c = (Color){1.0f, 0.5f, 0.0f, 1.0f};
            }
            else
            {
                c = (Color){0.8f, 0.0f, 1.0f, 1.0f};
            }

            glPushMatrix();
            glTranslatef(p->pos.x, p->pos.y, 0.0f);
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            render_triangle(maths_vec2(0, 0), p->radius * 1.4f, c);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            render_circle(p->pos, p->radius * 2.5f, (Color){c.r, c.g, c.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Color projectileColor = p->fromPlayer ? (Color){0.3f, 0.95f, 1.0f, 0.9f} : (Color){1.0f, 0.8f, 0.0f, 0.9f};

            render_circle(p->pos, p->radius, projectileColor, 14);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Color haloColor = p->fromPlayer ? (Color){0.3f, 0.85f, 1.0f, 0.28f} : (Color){1.0f, 0.75f, 0.0f, 0.24f};
            render_circle(p->pos, p->radius * 2.2f, haloColor, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}
