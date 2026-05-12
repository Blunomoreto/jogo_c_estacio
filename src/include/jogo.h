#ifndef JOGO_H
#define JOGO_H

#include "configuracao.h"
#include "persistencia.h"
#include "tipos.h"

void jogo_iniciar(Jogo *g, int width, int height);
void jogo_atualizar(Jogo *g, float dt);
void jogo_renderizar(Jogo *g);
void jogo_reiniciar(Jogo *g);
void jogo_tecla_pressionada(Jogo *g, unsigned char key, int x, int y);
void jogo_tecla_levantada(Jogo *g, unsigned char key, int x, int y);
void jogo_especial_pressionado(Jogo *g, int key, int x, int y);
void jogo_especial_levantado(Jogo *g, int key, int x, int y);
void jogo_mouse_pressionado(Jogo *g, int button, int state, int x, int y);
void jogo_mouse_movido(Jogo *g, int x, int y);
void jogo_iniciar_frame(Jogo *g);

#endif