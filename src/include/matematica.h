#ifndef MATEMATICA_H
#define MATEMATICA_H

#include "tipos.h"

Vetor2D matematica_vetor2d(float x, float y);
Vetor2D matematica_vetor2d_adicao(Vetor2D a, Vetor2D b);
Vetor2D matematica_vetor2d_subtracao(Vetor2D a, Vetor2D b);
Vetor2D matematica_vetor2d_multiplicacao(Vetor2D v, float s);
Vetor2D matematica_vetor2d_normalizar(Vetor2D v);
Vetor2D matematica_mouse_para_mundo(Game *g);

float matematica_float_aleatorio(float minV, float maxV);
float matematica_vetor2d_len(Vetor2D v);
float matematica_vetor2d_distance_sq(Vetor2D a, Vetor2D b);
float matematica_limite_min_max(float a, float min, float max);
float matematica_limite_min(float a, float min);
float matematica_limite_max(float a, float max);

#endif
