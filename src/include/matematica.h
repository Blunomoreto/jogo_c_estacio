#ifndef MATEMATICA_H
#define MATEMATICA_H

#include "tipos.h"

Vetor2D matematica_vetor2d(float x, float y);
Vetor2D matematica_vetor2d_adicao(Vetor2D vetor1, Vetor2D vetor2);
Vetor2D matematica_vetor2d_subtracao(Vetor2D vetor1, Vetor2D vetor2);
Vetor2D matematica_vetor2d_multiplicacao(Vetor2D vetor, float multiplicador);
Vetor2D matematica_vetor2d_normalizar(Vetor2D vetor);
Vetor2D matematica_mouse_para_mundo(Jogo *jogo);

float matematica_vetor2d_len(Vetor2D vetor);
float matematica_vetor2d_distance_sq(Vetor2D vetor1, Vetor2D vetor2);
float matematica_float_aleatorio_alcance(float min, float max);
float matematica_limite_min_max(float valor, float min, float max);
float matematica_limite_min(float valor, float min);
float matematica_limite_max(float valor, float max);

#endif
