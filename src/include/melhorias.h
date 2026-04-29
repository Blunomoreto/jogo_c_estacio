#ifndef UPGRADES_H
#define UPGRADES_H

#include "jogo.h"

void melhorias_preencher_opcoes(UpgradeOption *o, UpgradeType type);
void melhorias_rolar_opcoes(Game *g);
void melhorias_aplicar(Game *g, UpgradeType t);
void melhorias_desenhar_icone(UpgradeType t, float x, float y);
void melhorias_escolher(Game *g, int idx);

#endif
