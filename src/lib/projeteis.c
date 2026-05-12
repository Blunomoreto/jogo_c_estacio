#include "projeteis.h"
#include "matematica.h"

#include <string.h>

void projeteis_criar(Jogo *jogo, Vetor2D posicao, Vetor2D direcao, int do_jogador, float velocidade, float dano, float raio, float tempo_vida, TipoGuianca guianca, int identificador_alvo, float aceleracao_lateral_maxima)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &jogo->projetis[i];
        if (!p->ativo)
        {
            memset(p, 0, sizeof(*p));
            p->ativo = 1;
            p->vem_do_jogador = do_jogador;
            p->pos = posicao;
            p->vel = matematica_vetor2d_multiplicacao(direcao, velocidade);
            p->raio = raio;
            p->vida = tempo_vida;
            p->dano = dano;

            p->guianca = guianca;
            p->indice_alvo = identificador_alvo;
            p->aceleracao_lateral = 0.0f;
            p->tempo_com_combustivel = (guianca != GUIANCA_NENHUMA) ? MISSIL_TEMPO_COMBUSTIVEL : 0.0f;
            p->distancia_anterior = 99999.0f;
            p->errou = 0;
            p->temporizador_deteccao = 0.0f;
            p->aceleracao_lateral_max = aceleracao_lateral_maxima;
            return;
        }
    }
}
