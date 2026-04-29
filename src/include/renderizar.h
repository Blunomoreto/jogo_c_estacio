#ifndef RENDERIZAR_H
#define RENDERIZAR_H

#include "tipos.h"

struct Game;
struct Enemy;

void renderizar_perspectiva_ortografica(struct Game *g);
void renderizar_retangulo(float x, float y, float w, float h, Color c);
void renderizar_circulo(Vetor2D p, float radius, Color c, int segments);
void renderizar_triangulo(Vetor2D p, float size, Color c);
void renderizar_losangulo(Vetor2D p, float size, Color c);
void renderizar_estrela(Vetor2D p, float size, Color c);
void renderizar_pentagono(Vetor2D p, float size, Color c);

void renderizar_fundo(struct Game *g);
void renderizar_plataformas(struct Game *g);
void renderizar_jogador(struct Game *g);
void renderizar_inimigo(struct Enemy *e);
void renderizar_projeteis(struct Game *g);
void renderizar_particulas(struct Game *g);

#endif
