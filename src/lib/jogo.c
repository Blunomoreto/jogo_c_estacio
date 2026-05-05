#include "jogo.h"
#include "audio.h"
#include "cenario.h"
#include "configuracao.h"
#include "desenhar.h"
#include "imagem.h"
#include "inimigo.h"
#include "interface.h"
#include "matematica.h"
#include "persistencia.h"
#include "gameplay.h"
#include "renderizar.h"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void game_reset_player(Game *jogo)
{
    memset(&jogo->player, 0, sizeof(jogo->player));

    jogo->player.pos = matematica_vetor2d((float)jogo->width * 0.5f, ALTURA_CHAO - 16.0f);
    jogo->player.size = 16.0f;
    jogo->player.hp = JOGADOR_INICIAL_VIDA;
    jogo->player.maxHp = JOGADOR_INICIAL_VIDA;
    jogo->player.speed = JOGADOR_INICIAL_VELOCIDADE;
    jogo->player.damage = JOGADOR_INICIAL_DANO;
    jogo->player.fireRate = JOGADOR_INICIAL_TAXA_DISPARO;
    jogo->player.fireCooldown = 0.0f;
    jogo->player.projectileSpeed = JOGADOR_INICIAL_VELOCIDADE_PROJETIL;
    jogo->player.velY = 0.0f;
    jogo->player.isOnGround = 1;
    jogo->player.jumpPressedTime = 0.0f;

    jogo->player.hasPP = 0;
    jogo->player.hasAPNG = 0;
    jogo->player.guidedAmmo = 0;
    jogo->player.maxGuidedAmmo = 0;
    jogo->player.maxLatAccel = JOGADOR_INICIAL_ACELERACAO_LATERAL;
}

void jogo_iniciar(Game *jogo, int largura, int altura)
{
    memset(jogo, 0, sizeof(*jogo));
    srand((unsigned int)time(NULL));

    jogo->width = largura;
    jogo->height = altura;
    jogo->screen = SCREEN_MENU;
    jogo->running = 1;
    jogo->audioEnabled = 1;
    jogo->difficulty = 1;
    jogo->scorePage = 0;
    jogo->scorePageSize = 10;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    persistencia_carregar_stats(&jogo->highScore, &jogo->maxWaveEver);
    persistencia_carregar_configuracoes(&jogo->audioEnabled, &jogo->difficulty);
    interface_refrescar_pontuacoes_maximas(jogo);
    interface_refrescar_pontuacoes(jogo);

    jogo->bgTexture = imagem_carregar_ppm("assets/images/background.ppm", &jogo->bgTextureLoaded);
    audio_inicializar();
    audio_definir_ativacao(jogo->audioEnabled);
}

void jogo_atualizar(Game *jogo, float delta_tempo)
{
    if (!jogo->running)
        return;

    jogo->deltaTime = delta_tempo;

    jogo->upgradeFlash -= delta_tempo * 1.7f;
    if (jogo->upgradeFlash < 0.0f)
        jogo->upgradeFlash = 0.0f;
    jogo->lastUpgradeTimer -= delta_tempo;
    if (jogo->lastUpgradeTimer < 0.0f)
        jogo->lastUpgradeTimer = 0.0f;
    jogo->toastTimer -= delta_tempo;
    if (jogo->toastTimer < 0.0f)
        jogo->toastTimer = 0.0f;

    if (jogo->screen == SCREEN_PLAYING)
        gameplay_atualizar_playing(jogo, delta_tempo);
}

void jogo_renderizar(Game *jogo)
{
    int i;

    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderizar_perspectiva_ortografica(jogo);
    renderizar_fundo(jogo);

    if (jogo->screen == SCREEN_MENU)
        desenhar_menu(jogo);

    if (jogo->screen == SCREEN_SCORES)
        desenhar_placar(jogo);

    if (jogo->screen == SCREEN_OPTIONS)
        desenhar_opcoes(jogo);

    if (jogo->screen == SCREEN_PLAYING || jogo->screen == SCREEN_PAUSED ||
        jogo->screen == SCREEN_UPGRADE || jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE)
    {
        renderizar_plataformas(jogo);
        for (i = 0; i < MAXIMO_INIMIGOS; ++i)
        {
            if (jogo->enemies[i].active)
                renderizar_inimigo(&jogo->enemies[i]);
        }
        renderizar_projeteis(jogo);
        renderizar_particulas(jogo);
        renderizar_jogador(jogo);
        desenhar_hud(jogo);
        desenhar_vida_boss(jogo);
        if (jogo->screen == SCREEN_PLAYING)
            desenhar_mira(jogo);
    }

    if (jogo->screen == SCREEN_PAUSED)
        desenhar_pausa(jogo);

    if (jogo->screen == SCREEN_UPGRADE)
        desenhar_melhorias_tela(jogo);

    if (jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE)
        desenhar_fim(jogo);

    desenhar_flash(jogo);

    glutSwapBuffers();
}

void jogo_reiniciar(Game *jogo)
{
    game_reset_player(jogo);
    cenario_limpar_entidades(jogo);

    jogo->wave = 1;
    jogo->wavesToWin = inimigo_ondas_vitoria(jogo->difficulty);
    jogo->enemiesRemaining = 0;
    jogo->timeLeft = inimigo_tempo_inicio(jogo->difficulty);
    jogo->elapsed = 0.0f;
    jogo->score = 0;
    jogo->gold = 0;
    jogo->damageFlash = 0.0f;
    jogo->upgradeFlash = 0.0f;
    jogo->upgradeHover = -1;
    jogo->lastUpgradeTimer = 0.0f;
    memset(jogo->lastUpgrade, 0, sizeof(jogo->lastUpgrade));
    jogo->toastTimer = 0.0f;
    memset(jogo->toastMessage, 0, sizeof(jogo->toastMessage));
    jogo->screen = SCREEN_PLAYING;
    jogo->enteringName = 0;
    jogo->nameSaved = 0;

    memset(jogo->playerName, 0, sizeof(jogo->playerName));
    snprintf(jogo->playerName, sizeof(jogo->playerName), "Player");

    cenario_criar_onda(jogo);
    audio_tocar_musica();
}
