#include "particulas.h"
#include "matematica.h"

#include <math.h>

void particulas_criar(Jogo *jogo, Vetor2D vetor, int quantidade, Cor cor)
{
    int i;
    for (i = 0; i < quantidade; ++i)
    {
        int j;
        for (j = 0; j < MAXIMO_PARTICULAS; ++j)
        {
            Particula *pt = &jogo->particulas[j];
            if (!pt->ativo)
            {
                float ang = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
                float spd = matematica_float_aleatorio_alcance(30.0f, 180.0f);
                pt->ativo = 1;
                pt->pos = vetor;
                pt->vel = matematica_vetor2d(cosf(ang) * spd, sinf(ang) * spd);
                pt->tamanho = matematica_float_aleatorio_alcance(2.0f, 5.0f);
                pt->vida = matematica_float_aleatorio_alcance(0.2f, 0.6f);
                pt->cor = cor;
                break;
            }
        }
    }
}
