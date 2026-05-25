#ifndef RENDERIZAR_H
#define RENDERIZAR_H

#include "tipos.h"

struct Jogo;
struct Inimigo;

void renderizar_perspectiva_ortografica(struct Jogo *jogo);
void renderizar_retangulo(float x, float y, float largura, float altura, Cor cor);
void renderizar_circulo(Vetor2D vetor, float raio, Cor cor, int segmentos);
void renderizar_triangulo(Vetor2D vetor, float tamanho, Cor cor);
void renderizar_losangulo(Vetor2D vetor, float tamanho, Cor cor);
void renderizar_estrela(Vetor2D vetor, float tamanho, Cor cor);
void renderizar_pentagono(Vetor2D vetor, float tamanho, Cor cor);
void renderizar_iniciar_rotacao(float pivot_x, float pivot_y, float angulo_graus);
void renderizar_terminar_rotacao(void);
void renderizar_fundo(struct Jogo *jogo);
void renderizar_plataformas(struct Jogo *jogo);
void renderizar_jogador(struct Jogo *jogo);
void renderizar_inimigo(struct Inimigo *inimigo);
void renderizar_projeteis(struct Jogo *jogo);
void renderizar_particulas(struct Jogo *jogo);
void renderizar_sobreposicao_vulkan(struct Jogo *jogo);

#endif
