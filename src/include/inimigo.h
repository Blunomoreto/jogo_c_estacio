#ifndef INIMIGO_H
#define INIMIGO_H

#include "tipos.h"

Vetor2D inimigo_posicao(const Enemy *e);
Vetor2D inimigo_velocidade(const Enemy *e);
Vetor2D inimigo_aceleracao(const Enemy *e);

const char *inimigo_nome_dificuldade(int difficulty);

float inimigo_multiplicador_vida(int difficulty);
float inimigo_multiplicador_dano(int difficulty);
float inimigo_multiplicador_pontos(int difficulty);
float inimigo_taxa_disparo(int difficulty);
float inimigo_tempo_inicio(int difficulty);

int inimigo_ondas_vitoria(int difficulty);

#endif
