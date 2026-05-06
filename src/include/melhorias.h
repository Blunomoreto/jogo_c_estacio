#ifndef MELHORIAS_H
#define MELHORIAS_H

#include "tipos.h"

void melhorias_preencher_opcoes(UpgradeOption *opcoes, UpgradeType tipo);
void melhorias_rolar_opcoes(Game *jogo);
void melhorias_aplicar(Game *jogo, UpgradeType tipo);
void melhorias_escolher(Game *jogo, int identificador);

#endif
