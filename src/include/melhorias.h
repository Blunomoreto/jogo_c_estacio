#ifndef UPGRADES_H
#define UPGRADES_H

#include "jogo.h"

void melhorias_preencher_opcoes(OpcaoMelhoria *o, TipoMelhoria type);
void melhorias_rolar_opcoes(Jogo *g);
void melhorias_aplicar(Jogo *g, TipoMelhoria t);
void melhorias_escolher(Jogo *g, int idx);

#endif