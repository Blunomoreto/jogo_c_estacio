#include "interface.h"
#include "persistencia.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>

void interface_refrescar_pontuacoes_maximas(Jogo *g)
{
    g->contadorTopPontuacoes = persistencia_carregar_pontuacoes_altas(g->topPontuacoes, 5);
}

void interface_refrescar_pontuacoes(Jogo *g)
{
    g->contadorTodasPontuacoes = persistencia_carregar_pontuacoes(g->todasPontuacoes, 64);
    if (g->paginaPontuacao < 0)
        g->paginaPontuacao = 0;
}

void interface_notificar(Jogo *g, const char *msg)
{
    snprintf(g->mensagemToast, sizeof(g->mensagemToast), "%s", msg ? msg : "");
    g->temporizadorToast = 2.2f;
}

void interface_encerrar_partida(Jogo *g, TelaJogo fim)
{
    parar_musica();
    g->tela = fim;
    g->inserindoNome = 1;
    g->nomeSalvo = 0;

    if (g->pontuacao > g->maiorPontuacao)
        g->maiorPontuacao = g->pontuacao;
    if (g->onda > g->maiorOndaAtingida)
        g->maiorOndaAtingida = g->onda;
    persistencia_salvar_stats(g->maiorPontuacao, g->maiorOndaAtingida);
    interface_refrescar_pontuacoes_maximas(g);
}