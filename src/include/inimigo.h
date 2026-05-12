#ifndef INIMIGO_H
#define INIMIGO_H

#include "tipos.h"

Vetor2D inimigo_posicao(const Inimigo *e);

const char *inimigo_nome_dificuldade(int dificuldade);

float inimigo_multiplicador_vida(int dificuldade);
float inimigo_multiplicador_dano(int dificuldade);
float inimigo_multiplicador_pontos(int dificuldade);
float inimigo_taxa_disparo(int dificuldade);
float inimigo_tempo_inicio(int dificuldade);

int inimigo_ondas_vitoria(int dificuldade);

#endif