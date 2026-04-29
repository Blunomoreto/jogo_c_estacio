#ifndef INTERFACE_H
#define INTERFACE_H

#include "jogo.h"

void interface_refrescar_pontuacoes_maximas(Game *g);
void interface_refrescar_pontuacoes(Game *g);
void interface_notificar(Game *g, const char *msg);
void interface_encerrar_partida(Game *g, GameScreen fim);

#endif
