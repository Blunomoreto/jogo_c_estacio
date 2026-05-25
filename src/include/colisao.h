#ifndef COLISAO_H
#define COLISAO_H

#include "tipos.h"

int colisao_circulo_vs_circulo(Vetor2D posicaoA, float raioA, Vetor2D posicaoB, float raioB);
int colisao_circulo_vs_retangulo(Vetor2D posicaoA, float raioA, Vetor2D cantoSuperiorEsquerdo, Vetor2D cantoSuperiorDireito);

Vetor2D colisao_resolver_circulo_vs_obb(Vetor2D centro_circulo, float raio, Vetor2D centro_retangulo, float largura, float altura, float angulo_graus, int *colidiu);

#endif
