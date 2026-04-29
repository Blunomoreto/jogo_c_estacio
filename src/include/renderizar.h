#ifndef RENDERIZAR_H
#define RENDERIZAR_H

#include "tipos.h"

struct Game;

void renderizar_texto(float x, float y, const char *text, void *font, float r, float g, float b);
void renderizar_texto_limites(float x, float y, const char *text, void *font, float r, float g, float b, float maxW);
void renderizar_retangulo(float x, float y, float w, float h, Color c);
void renderizar_circulo(Vetor2D p, float radius, Color c, int segments);
void renderizar_triangulo(Vetor2D p, float size, Color c);
void renderizar_losangulo(Vetor2D p, float size, Color c);
void renderizar_estrela(Vetor2D p, float size, Color c);
void renderizar_pentagono(Vetor2D p, float size, Color c);
void renderizar_perspectiva_ortografica(struct Game *g);

#endif
