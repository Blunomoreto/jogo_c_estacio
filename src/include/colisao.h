#ifndef COLISAO_H
#define COLISAO_H

#include "matematica.h"

int colisao_circulo_vs_circulo(Vetor2D a, float ra, Vetor2D b, float rb);
int colisao_circulo_vs_retangulo(Vetor2D c, float r, Vetor2D bmin, Vetor2D bmax);
int colisao_dentro_do_retangulo(float x, float y, float rx, float ry, float rw, float rh);

#endif
