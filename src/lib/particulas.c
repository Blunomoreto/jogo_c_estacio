#include "particulas.h"
#include "matematica.h"

#include <math.h>

void particulas_criar(Jogo *g, Vetor2D p, int count, Color color)
{
    int i;
    for (i = 0; i < count; ++i)
    {
        int j;
        for (j = 0; j < MAXIMO_PARTICULAS; ++j)
        {
            Particula *pt = &g->particulas[j];
            if (!pt->ativo)
            {
                float ang = matematica_float_aleatorio(0.0f, 2.0f * (float)M_PI);
                float spd = matematica_float_aleatorio(30.0f, 180.0f);
                pt->ativo = 1;
                pt->pos = p;
                pt->vel = matematica_vetor2d(cosf(ang) * spd, sinf(ang) * spd);
                pt->tamanho = matematica_float_aleatorio(2.0f, 5.0f);
                pt->vida = matematica_float_aleatorio(0.2f, 0.6f);
                pt->cor = color;
                break;
            }
        }
    }
}