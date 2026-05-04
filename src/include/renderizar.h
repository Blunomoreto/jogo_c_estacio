#ifndef RENDERIZAR_H
#define RENDERIZAR_H

#include "tipos.h"

struct Game;
struct Enemy;

void renderizar_perspectiva_ortografica(struct Game *jogo);
void renderizar_retangulo(float x, float y, float largura, float altura, Color cor);
void renderizar_circulo(Vetor2D vetor, float raio, Color cor, int segmentos);
void renderizar_triangulo(Vetor2D vetor, float tamanho, Color cor);
void renderizar_losangulo(Vetor2D vetor, float tamanho, Color cor);
void renderizar_estrela(Vetor2D vetor, float tamanho, Color cor);
void renderizar_pentagono(Vetor2D vetor, float tamanho, Color cor);
void renderizar_fundo(struct Game *jogo);
void renderizar_plataformas(struct Game *jogo);
void renderizar_jogador(struct Game *jogo);
void renderizar_inimigo(struct Enemy *inimigo);
void renderizar_projeteis(struct Game *jogo);
void renderizar_particulas(struct Game *jogo);

#endif
