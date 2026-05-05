#ifndef JOGO_H
#define JOGO_H

#include "configuracao.h"
#include "persistencia.h"
#include "tipos.h"

void jogo_iniciar(Game *jogo, int largura, int altura);
void jogo_atualizar(Game *jogo, float delta_tempo);
void jogo_renderizar(Game *jogo);
void jogo_reiniciar(Game *jogo);

#endif
