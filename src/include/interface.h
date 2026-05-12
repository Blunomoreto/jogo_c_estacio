#ifndef INTERFACE_H
#define INTERFACE_H

#include "jogo.h"

void interface_refrescar_pontuacoes_maximas(Jogo *g);
void interface_refrescar_pontuacoes(Jogo *g);
void interface_notificar(Jogo *g, const char *msg);
void interface_encerrar_partida(Jogo *g, TelaJogo fim);

#endif