#ifndef PROJETEIS_H
#define PROJETEIS_H

#include "tipos.h"

void projeteis_criar(Game *jogo, Vetor2D posicao, Vetor2D direcao, int do_jogador, float velocidade, float dano, float raio, float tempo_vida, GuidanceType guianca, int identificador_alvo, float aceleracao_lateral_maxima);

#endif
