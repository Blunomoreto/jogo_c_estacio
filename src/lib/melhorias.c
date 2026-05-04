#include "melhorias.h"
#include "cenario.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

void melhorias_preencher_opcoes(UpgradeOption *opcoes, UpgradeType tipo)
{
    opcoes->type = tipo;
    switch (tipo)
    {
    case UPGRADE_DAMAGE:
        snprintf(opcoes->label, sizeof(opcoes->label), "Dano do Disparo");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Aumenta o dano do disparo");
        break;
    case UPGRADE_FIRE_RATE:
        snprintf(opcoes->label, sizeof(opcoes->label), "Taxa de Disparo");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Aumenta a taxa de disparo");
        break;
    case UPGRADE_SPEED:
        snprintf(opcoes->label, sizeof(opcoes->label), "Velocidade");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Aumenta a velocidade de movimento");
        break;
    case UPGRADE_HEAL:
        snprintf(opcoes->label, sizeof(opcoes->label), "Aumento de Vida");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Recupera e adiciona mais 50 de vida");
        break;
    case UPGRADE_TIME:
        snprintf(opcoes->label, sizeof(opcoes->label), "Aumento de Tempo");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Acrescenta 20 segundos ao timer");
        break;
    case UPGRADE_GUIDANCE_PP:
        snprintf(opcoes->label, sizeof(opcoes->label), "Balas Guiadas");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Desbloqueia projeteis guiados com 5 balas guiadas");
        break;
    case UPGRADE_GUIDANCE_APNG:
        snprintf(opcoes->label, sizeof(opcoes->label), "Balas Guiadas+");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Melhora os projeteis guiados e adiciona mais 5 balas guiadas");
        break;
    case UPGRADE_AMMO:
        snprintf(opcoes->label, sizeof(opcoes->label), "Aumento de Municao");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Adiciona mais 10 balas guiadas");
        break;
    default:
        snprintf(opcoes->label, sizeof(opcoes->label), "Desconhecido");
        snprintf(opcoes->desc, sizeof(opcoes->desc), "Efeito desconhecido");
        break;
    }
}

void melhorias_rolar_opcoes(Game *jogo)
{
    int i;
    int used[UPGRADE_COUNT] = {0};

    if (jogo->player.hasPP)
        used[UPGRADE_GUIDANCE_PP] = 1;
    if (jogo->player.hasAPNG)
        used[UPGRADE_GUIDANCE_APNG] = 1;

    for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
    {
        UpgradeType t;
        int guard = 0;
        do
        {
            t = (UpgradeType)(rand() % UPGRADE_COUNT);
            if (t == UPGRADE_GUIDANCE_APNG && !jogo->player.hasPP)
                t = UPGRADE_DAMAGE;
            if ((t == UPGRADE_AMMO) && (!jogo->player.hasPP && !jogo->player.hasAPNG))
            {
                t = UPGRADE_DAMAGE;
            }
            guard++;
        } while (used[t] && guard < 32);
        used[t] = 1;
        melhorias_preencher_opcoes(&jogo->upgrades[i], t);
    }
}

void melhorias_aplicar(Game *jogo, UpgradeType t)
{
    switch (t)
    {
    case UPGRADE_DAMAGE:
        jogo->player.damage += 7.5f;
        break;
    case UPGRADE_FIRE_RATE:
        jogo->player.fireRate *= 0.8f;
        if (jogo->player.fireRate < 0.1f)
        {
            jogo->player.fireRate = 0.1f;
        }
        break;
    case UPGRADE_SPEED:
        jogo->player.speed += 20.0f;
        break;
    case UPGRADE_HEAL:
        jogo->player.maxHp += 50.0f;
        jogo->player.hp = jogo->player.maxHp;
        break;
    case UPGRADE_TIME:
        jogo->timeLeft += 20.0f;
        break;
    case UPGRADE_GUIDANCE_PP:
        jogo->player.hasPP = 1;
        jogo->player.maxGuidedAmmo += 5;
        jogo->player.guidedAmmo = jogo->player.maxGuidedAmmo;
        if (jogo->player.maxLatAccel < 100.0f)
            jogo->player.maxLatAccel = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case UPGRADE_GUIDANCE_APNG:
        if (!jogo->player.hasPP)
        {
            jogo->player.hasPP = 1;
            jogo->player.maxGuidedAmmo += 10;
        }
        jogo->player.hasAPNG = 1;
        jogo->player.maxGuidedAmmo += 5;
        jogo->player.guidedAmmo = jogo->player.maxGuidedAmmo;
        if (jogo->player.maxLatAccel < 100.0f)
            jogo->player.maxLatAccel = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case UPGRADE_AMMO:
        jogo->player.maxGuidedAmmo += 10;
        jogo->player.guidedAmmo = jogo->player.maxGuidedAmmo;
        break;
    default:
        break;
    }
}

void melhorias_escolher(Game *jogo, int identificador)
{
    if (identificador < 0 || identificador >= MAXIMO_OPCOES_UPGRADE)
    {
        return;
    }

    melhorias_aplicar(jogo, jogo->upgrades[identificador].type);
    snprintf(jogo->lastUpgrade, sizeof(jogo->lastUpgrade), "%s", jogo->upgrades[identificador].label);
    jogo->lastUpgradeTimer = 2.5f;
    jogo->upgradeFlash = 1.0f;
    audio_tocar_som_tiro_disparo();

    jogo->player.guidedAmmo = jogo->player.maxGuidedAmmo;

    jogo->wave += 1;
    cenario_criar_onda(jogo);
    jogo->screen = SCREEN_PLAYING;
}
