#ifndef INTERFACE_H
#define INTERFACE_H

#include "tipos.h"

void interface_refrescar_pontuacoes_maximas(Jogo *jogo);
void interface_refrescar_pontuacoes(Jogo *jogo);
void interface_notificar(Jogo *jogo, const char *mensagem);
void interface_encerrar_partida(Jogo *jogo, TelaJogo fim);

#endif
