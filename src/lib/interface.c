#include "interface.h"
#include "persistencia.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>

void interface_refrescar_pontuacoes_maximas(Game *jogo)
{
    jogo->topScoreCount = persistencia_carregar_pontuacoes_altas(jogo->topScores, 5);
}

void interface_refrescar_pontuacoes(Game *jogo)
{
    jogo->allScoreCount = persistencia_carregar_pontuacoes(jogo->allScores, 64);
    if (jogo->scorePage < 0)
        jogo->scorePage = 0;
}

void interface_notificar(Game *jogo, const char *mensagem)
{
    snprintf(jogo->toastMessage, sizeof(jogo->toastMessage), "%s", mensagem ? mensagem : "");
    jogo->toastTimer = 2.2f;
}

void interface_encerrar_partida(Game *jogo, GameScreen fim)
{
    audio_parar_musica();
    jogo->screen = fim;
    jogo->enteringName = 1;
    jogo->nameSaved = 0;

    if (jogo->score > jogo->highScore)
        jogo->highScore = jogo->score;
    if (jogo->wave > jogo->maxWaveEver)
        jogo->maxWaveEver = jogo->wave;
    persistencia_salvar_stats(jogo->highScore, jogo->maxWaveEver);
    interface_refrescar_pontuacoes_maximas(jogo);
}
