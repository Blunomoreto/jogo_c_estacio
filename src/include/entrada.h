#ifndef ENTRADA_H
#define ENTRADA_H

#include "tipos.h"

void entrada_tecla_pressionada(Jogo *jogo, unsigned char tecla, int x, int y);
void entrada_tecla_levantada(Jogo *jogo, unsigned char tecla, int x, int y);
void entrada_especial_pressionado(Jogo *jogo, int tecla, int x, int y);
void entrada_especial_levantado(Jogo *jogo, int tecla, int x, int y);
void entrada_mouse_pressionado(Jogo *jogo, int botao, int estado, int x, int y);
void entrada_mouse_movido(Jogo *jogo, int x, int y);
void entrada_iniciar_frame(Jogo *jogo);

#endif
