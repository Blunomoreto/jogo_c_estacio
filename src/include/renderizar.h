#ifndef RENDERIZAR_H
#define RENDERIZAR_H

#include "tipos.h"

struct Jogo;
struct Inimigo;

void renderizar_perspectiva_ortografica(struct Jogo *g);
void renderizar_retangulo(float x, float y, float w, float h, Color c);
void renderizar_circulo(Vetor2D p, float radius, Color c, int segments);
void renderizar_triangulo(Vetor2D p, float size, Color c);
void renderizar_losangulo(Vetor2D p, float size, Color c);
void renderizar_estrela(Vetor2D p, float size, Color c);
void renderizar_pentagono(Vetor2D p, float size, Color c);

void renderizar_fundo(struct Jogo *g);
void renderizar_plataformas(struct Jogo *g);
void renderizar_jogador(struct Jogo *g);
void renderizar_inimigo(struct Inimigo *e);
void renderizar_projeteis(struct Jogo *g);
void renderizar_particulas(struct Jogo *g);

#endif