#include "interface.h"
#include "persistencia.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>

void interface_refrescar_pontuacoes_maximas(Game *g)
{
    g->topScoreCount = persistencia_carregar_pontuacoes_altas(g->topScores, 5);
}

void interface_refrescar_pontuacoes(Game *g)
{
    g->allScoreCount = persistencia_carregar_pontuacoes(g->allScores, 64);
    if (g->scorePage < 0)
        g->scorePage = 0;
}

void interface_notificar(Game *g, const char *msg)
{
    snprintf(g->toastMessage, sizeof(g->toastMessage), "%s", msg ? msg : "");
    g->toastTimer = 2.2f;
}

void interface_encerrar_partida(Game *g, GameScreen fim)
{
    audio_parar_musica();
    g->screen = fim;
    g->enteringName = 1;
    g->nameSaved = 0;

    if (g->score > g->highScore)
        g->highScore = g->score;
    if (g->wave > g->maxWaveEver)
        g->maxWaveEver = g->wave;
    persistencia_salvar_stats(g->highScore, g->maxWaveEver);
    interface_refrescar_pontuacoes_maximas(g);
}
