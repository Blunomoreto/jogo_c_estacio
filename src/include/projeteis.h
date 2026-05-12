#ifndef PROJETEIS_H
#define PROJETEIS_H

#include "jogo.h"

void projeteis_criar(Jogo *g, Vetor2D pos, Vetor2D dir, int fromPlayer, float speed, float damage, float radius, float life, TipoOrientacao guidance, int targetIdx, float maxLatAccel);

#endif