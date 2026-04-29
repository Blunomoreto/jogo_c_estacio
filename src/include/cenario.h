#ifndef CENARIO_H
#define CENARIO_H

#include "jogo.h"

void cenario_limpar_entidades(Game *g);
void cenario_limpar_plataformas(Game *g);
void cenario_criar_onda(Game *g);
void cenario_criar_plataformas(Game *g);
void cenario_desenhar_plataformas(Game *g);
void cenario_desenhar_fundo(Game *g);

#endif
