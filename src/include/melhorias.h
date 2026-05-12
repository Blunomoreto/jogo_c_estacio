#ifndef MELHORIAS_H
#define MELHORIAS_H

#include "tipos.h"

void melhorias_preencher_opcoes(OpcaoMelhoria *opcoes, TipoMelhoria tipo);
void melhorias_rolar_opcoes(Jogo *jogo);
void melhorias_aplicar(Jogo *jogo, TipoMelhoria tipo);
void melhorias_escolher(Jogo *jogo, int identificador);

#endif
