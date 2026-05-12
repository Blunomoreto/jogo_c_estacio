#ifndef JOGO_H
#define JOGO_H

#include "tipos.h"

void jogo_iniciar(Jogo *jogo, int largura, int altura);
void jogo_atualizar(Jogo *jogo, float delta_tempo);
void jogo_renderizar(Jogo *jogo);
void jogo_reiniciar(Jogo *jogo);

#endif
