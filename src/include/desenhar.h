#ifndef DESENHAR_H
#define DESENHAR_H

#include "jogo.h"

void desenhar_texto(float x, float y, const char *texto, void *fonte, float r, float g, float b);
void desenhar_texto_limites(float x, float y, const char *texto, void *fonte, float r, float g, float b, float largMax);
void desenhar_hud(Game *jogo);
void desenhar_mira(Game *jogo);
void desenhar_vida_boss(Game *jogo);
void desenhar_icone_melhoria(UpgradeType tipo, float x, float y);
void desenhar_menu(Game *jogo);
void desenhar_placar(Game *jogo);
void desenhar_opcoes(Game *jogo);
void desenhar_pausa(Game *jogo);
void desenhar_melhorias_tela(Game *jogo);
void desenhar_fim(Game *jogo);
void desenhar_flash(Game *jogo);

#endif
