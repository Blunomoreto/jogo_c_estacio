#ifndef INTERFACE_H
#define INTERFACE_H

#include "tipos.h"

void interface_refrescar_pontuacoes_maximas(Game *jogo);
void interface_refrescar_pontuacoes(Game *jogo);
void interface_notificar(Game *jogo, const char *mensagem);
void interface_encerrar_partida(Game *jogo, GameScreen fim);

#endif
