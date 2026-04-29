#ifndef INTERFACE_H
#define INTERFACE_H

#include "jogo.h"

void interface_desenhar_jogador(Game *g);
void interface_desenhar_inimigo(Enemy *e);
void interface_desenhar_vida_boss(Game *g);
void interface_desenhar_hud(Game *g);
void interface_desenhar_mira(Game *g);
void interface_refrescar_pontuacoes_maximas(Game *g);
void interface_refrescar_pontuacoes(Game *g);
void interface_desenhar_notificacao(Game *g, const char *msg);
void interface_desenhar_tela_fim(Game *g, GameScreen end);

#endif
