#ifndef PROJETEIS_H
#define PROJETEIS_H

#include "jogo.h"

void projeteis_criar(Game *g, Vetor2D pos, Vetor2D dir, int fromPlayer, float speed, float damage, float radius, float life, GuidanceType guidance, int targetIdx, float maxLatAccel);
void projeteis_desenhar(Game *g);

#endif
