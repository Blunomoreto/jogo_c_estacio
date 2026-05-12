#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "tipos.h"

void persistencia_carregar_stats(int *highScore, int *maxWave);
void persistencia_salvar_stats(int highScore, int maxWave);
void persistencia_apor_pontuacao(const char *name, int score, int wave);
void persistencia_limpar_pontuacoes(void);
void persistencia_carregar_configuracoes(int *audioEnabled, int *difficulty);
void persistencia_salvar_configuracoes(int audioEnabled, int difficulty);

int persistencia_carregar_pontuacoes_altas(ScoreEntry *outEntries, int maxEntries);
int persistencia_carregar_pontuacoes(ScoreEntry *outEntries, int maxEntries);

#endif
