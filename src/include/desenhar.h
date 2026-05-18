#ifndef DESENHAR_H
#define DESENHAR_H

#include "tipos.h"

void desenhar_texto(float x, float y, const char *texto, void *fonte, float r, float g, float b);
void desenhar_texto_centralizado(float x, float y, const char *texto, void *fonte, float r, float g, float b);
void desenhar_texto_limites(float x, float y, const char *texto, void *fonte, float r, float g, float b, float largMax);
void desenhar_hud(Jogo *jogo);
void desenhar_mira(Jogo *jogo);
void desenhar_vida_boss(Jogo *jogo);
void desenhar_icone_melhoria(TipoMelhoria tipo, float x, float y);
void desenhar_menu(Jogo *jogo);
void desenhar_placar(Jogo *jogo);
void desenhar_opcoes(Jogo *jogo);
void desenhar_pausa(Jogo *jogo);
void desenhar_melhorias_tela(Jogo *jogo);
void desenhar_fim(Jogo *jogo);
void desenhar_flash(Jogo *jogo);

#endif
