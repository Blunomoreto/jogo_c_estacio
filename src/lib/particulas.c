#include "particulas.h"
#include "matematica.h"
#include "renderizar.h"

#include <math.h>

void particulas_criar(Game *g, Vetor2D p, int count, Color color)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        int j;
        for (j = 0; j < MAXIMO_PARTICULAS; ++j)
        {
            Particle *pt = &g->particles[j];
            if (!pt->active)
            {
                float ang = matematica_float_aleatorio(0.0f, 2.0f * (float)M_PI);
                float spd = matematica_float_aleatorio(30.0f, 180.0f);
                pt->active = 1;
                pt->pos = p;
                pt->vel = matematica_vetor2d(cosf(ang) * spd, sinf(ang) * spd);
                pt->size = matematica_float_aleatorio(2.0f, 5.0f);
                pt->life = matematica_float_aleatorio(0.2f, 0.6f);
                pt->color = color;
                break;
            }
        }
    }
}

void particulas_desenhar(Game *g)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &g->particles[i];
        if (!pt->active)
        {
            continue;
        }
        renderizar_circulo(pt->pos, pt->size, (Color){pt->color.r, pt->color.g, pt->color.b, pt->color.a * (pt->life * 2.0f)}, 8);
    }
}
