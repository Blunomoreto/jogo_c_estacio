#include "projeteis.h"
#include "matematica.h"

#include <string.h>

void projeteis_criar(Game *jogo, Vetor2D posicao, Vetor2D direcao, int do_jogador, float velocidade, float dano, float raio, float tempo_vida, GuidanceType guianca, int identificador_alvo, float aceleracao_lateral_maxima)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &jogo->projectiles[i];
        if (!p->active)
        {
            memset(p, 0, sizeof(*p));
            p->active = 1;
            p->fromPlayer = do_jogador;
            p->pos = posicao;
            p->vel = matematica_vetor2d_multiplicacao(direcao, velocidade);
            p->radius = raio;
            p->life = tempo_vida;
            p->damage = dano;

            p->guidance = guianca;
            p->targetIdx = identificador_alvo;
            p->actualLatAccel = 0.0f;
            p->fuelTimer = (guianca != GUIDANCE_NONE) ? MISSIL_TEMPO_COMBUSTIVEL : 0.0f;
            p->prevDist = 99999.0f;
            p->missed = 0;
            p->sdTimer = 0.0f;
            p->maxLatAccel = aceleracao_lateral_maxima;
            return;
        }
    }
}
