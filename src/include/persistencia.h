#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "tipos.h"

void persistencia_carregar_stats(int *pontuacao_maxima, int *onda_maxima);
void persistencia_salvar_stats(int pontuacao_maxima, int onda_maxima);
void persistencia_apor_pontuacao(const char *nome, int pontuacao, int onda);
void persistencia_limpar_pontuacoes(void);
void persistencia_carregar_configuracoes(int *audio_ativado, int *dificuldade);
void persistencia_salvar_configuracoes(int audio_ativado, int dificuldade);

int persistencia_carregar_pontuacoes_altas(RegistroPontuacao *entradas_fora_lista, int entradas_maximas);
int persistencia_carregar_pontuacoes(RegistroPontuacao *entradas_fora_lista, int entradas_maximas);

#endif
