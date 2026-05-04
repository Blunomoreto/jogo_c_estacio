#ifndef JOGO_H
#define JOGO_H

#include "configuracao.h"
#include "persistencia.h"
#include "tipos.h"

void jogo_iniciar(Game *jogo, int largura, int altura);
void jogo_atualizar(Game *jogo, float delta_tempo);
void jogo_renderizar(Game *jogo);
void jogo_reiniciar(Game *jogo);
void jogo_tecla_pressionada(Game *jogo, unsigned char tecla, int x, int y);
void jogo_tecla_levantada(Game *jogo, unsigned char tecla, int x, int y);
void jogo_especial_pressionado(Game *jogo, int tecla, int x, int y);
void jogo_especial_levantado(Game *jogo, int tecla, int x, int y);
void jogo_mouse_pressionado(Game *jogo, int botao, int estado, int x, int y);
void jogo_mouse_movido(Game *jogo, int x, int y);
void jogo_iniciar_frame(Game *jogo);

#endif
