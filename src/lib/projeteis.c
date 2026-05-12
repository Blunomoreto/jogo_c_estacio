#include "projeteis.h"
#include "matematica.h"

#include <string.h>

void projeteis_criar(Jogo *jogo, Vetor2D posicao, Vetor2D direcao, int do_jogador, float velocidade, float dano, float raio, float tempo_vida, TipoGuianca guianca, int identificador_alvo, float aceleracao_lateral_maxima)
{
    int indice_projetil;
    for (indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; ++indice_projetil)
    {
        Projetil *projetil = &jogo->projetis[indice_projetil];
        if (!projetil->ativo)
        {
            memset(projetil, 0, sizeof(*projetil));
            projetil->ativo = 1;
            projetil->vem_do_jogador = do_jogador;
            projetil->pos = posicao;
            projetil->vel = matematica_vetor2d_multiplicacao(direcao, velocidade);
            projetil->raio = raio;
            projetil->vida = tempo_vida;
            projetil->dano = dano;

            projetil->guianca = guianca;
            projetil->indice_alvo = identificador_alvo;
            projetil->aceleracao_lateral = 0.0f;
            projetil->tempo_com_combustivel = (guianca != GUIANCA_NENHUMA) ? MISSIL_TEMPO_COMBUSTIVEL : 0.0f;
            projetil->distancia_anterior = 99999.0f;
            projetil->errou = 0;
            projetil->temporizador_deteccao = 0.0f;
            projetil->aceleracao_lateral_max = aceleracao_lateral_maxima;
            return;
        }
    }
}
