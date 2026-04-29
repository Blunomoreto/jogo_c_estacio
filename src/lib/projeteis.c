#include "projeteis.h"
#include "matematica.h"
#include "renderizar.h"

#include <GL/glut.h>
#include <math.h>
#include <string.h>

void projeteis_criar(Game *g, Vetor2D pos, Vetor2D dir, int fromPlayer, float speed, float damage, float radius, float life, GuidanceType guidance, int targetIdx, float maxLatAccel)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &g->projectiles[i];
        if (!p->active)
        {
            memset(p, 0, sizeof(*p));
            p->active = 1;
            p->fromPlayer = fromPlayer;
            p->pos = pos;
            p->vel = matematica_vetor2d_multiplicacao(dir, speed);
            p->radius = radius;
            p->life = life;
            p->damage = damage;

            p->guidance = guidance;
            p->targetIdx = targetIdx;
            p->actualLatAccel = 0.0f;
            p->fuelTimer = (guidance != GUIDANCE_NONE) ? MISSIL_TEMPO_COMBUSTIVEL : 0.0f;
            p->prevDist = 99999.0f;
            p->missed = 0;
            p->sdTimer = 0.0f;
            p->maxLatAccel = maxLatAccel;
            return;
        }
    }
}

void projeteis_desenhar(Game *g)
{
    int i;

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
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
            renderizar_triangulo(matematica_vetor2d(0, 0), p->radius * 1.4f, c);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(p->pos, p->radius * 2.5f, (Color){c.r, c.g, c.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Color projectileColor = p->fromPlayer ? (Color){0.3f, 0.95f, 1.0f, 0.9f} : (Color){1.0f, 0.8f, 0.0f, 0.9f};

            renderizar_circulo(p->pos, p->radius, projectileColor, 14);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Color haloColor = p->fromPlayer ? (Color){0.3f, 0.85f, 1.0f, 0.28f} : (Color){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(p->pos, p->radius * 2.2f, haloColor, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}
