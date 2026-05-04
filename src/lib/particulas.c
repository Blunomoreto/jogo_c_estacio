#include "particulas.h"
#include "matematica.h"

#include <math.h>

void particulas_criar(Game *jogo, Vetor2D vetor, int quantidade, Color cor)
{
    int i;
    for (i = 0; i < quantidade; ++i)
    {
        int j;
        for (j = 0; j < MAXIMO_PARTICULAS; ++j)
        {
            Particle *pt = &jogo->particles[j];
            if (!pt->active)
            {
                float ang = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
                float spd = matematica_float_aleatorio_alcance(30.0f, 180.0f);
                pt->active = 1;
                pt->pos = vetor;
                pt->vel = matematica_vetor2d(cosf(ang) * spd, sinf(ang) * spd);
                pt->size = matematica_float_aleatorio_alcance(2.0f, 5.0f);
                pt->life = matematica_float_aleatorio_alcance(0.2f, 0.6f);
                pt->color = cor;
                break;
            }
        }
    }
}
