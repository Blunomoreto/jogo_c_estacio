#ifndef COLISAO_H
#define COLISAO_H

#include "matematica.h"

int colisao_circulo_vs_circulo(Vetor2D posicaoA, float raioA, Vetor2D posicaoB, float raioB);
int colisao_circulo_vs_retangulo(Vetor2D posicaoA, float raioA, Vetor2D cantoSuperiorEsquerdo, Vetor2D cantoSuperiorDireito);

#endif
