#include "projeteis.h"
#include "matematica.h"

#include <string.h>

void projeteis_criar(Jogo *g, Vetor2D pos, Vetor2D dir, int fromPlayer, float speed, float damage, float radius, float life, TipoOrientacao guidance, int targetIdx, float maxLatAccel)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &g->projetis[i];
        if (!p->ativo)
        {
            memset(p, 0, sizeof(*p));
            p->ativo = 1;
            p->vemDoJogador = fromPlayer;
            p->pos = pos;
            p->vel = matematica_vetor2d_multiplicacao(dir, speed);
            p->raio = radius;
            p->vida = life;
            p->dano = damage;

            p->orientacao = guidance;
            p->indiceAlvo = targetIdx;
            p->aceleracaoLateralAtual = 0.0f;
            p->temporizadorCombustivel = (guidance != ORIENTACAO_NENHUMA) ? MISSIL_TEMPO_COMBUSTIVEL : 0.0f;
            p->distanciaAnterior = 99999.0f;
            p->errou = 0;
            p->temporizadorDeteccao = 0.0f;
            p->aceleracaoLateralMaxima = maxLatAccel;
            return;
        }
    }
}