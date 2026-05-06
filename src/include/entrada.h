#ifndef ENTRADA_H
#define ENTRADA_H

#include "tipos.h"

void entrada_tecla_pressionada(Game *jogo, unsigned char tecla, int x, int y);
void entrada_tecla_levantada(Game *jogo, unsigned char tecla, int x, int y);
void entrada_especial_pressionado(Game *jogo, int tecla, int x, int y);
void entrada_especial_levantado(Game *jogo, int tecla, int x, int y);
void entrada_mouse_pressionado(Game *jogo, int botao, int estado, int x, int y);
void entrada_mouse_movido(Game *jogo, int x, int y);
void entrada_iniciar_frame(Game *jogo);

#endif
