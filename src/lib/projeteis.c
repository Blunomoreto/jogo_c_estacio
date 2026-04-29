#include "projeteis.h"
#include "matematica.h"

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

