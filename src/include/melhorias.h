#ifndef UPGRADES_H
#define UPGRADES_H

#include "jogo.h"

void melhorias_preencher_opcoes(UpgradeOption *opcoes, UpgradeType tipo);
void melhorias_rolar_opcoes(Game *jogo);
void melhorias_aplicar(Game *jogo, UpgradeType tipo);
void melhorias_escolher(Game *jogo, int identificador);

#endif
