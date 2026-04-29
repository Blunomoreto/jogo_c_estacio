#include "melhorias.h"
#include "cenario.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

void melhorias_preencher_opcoes(UpgradeOption *o, UpgradeType type)
{
    o->type = type;
    switch (type)
    {
    case UPGRADE_DAMAGE:
        snprintf(o->label, sizeof(o->label), "Dano do Disparo");
        snprintf(o->desc, sizeof(o->desc), "Aumenta o dano do disparo");
        break;
    case UPGRADE_FIRE_RATE:
        snprintf(o->label, sizeof(o->label), "Taxa de Disparo");
        snprintf(o->desc, sizeof(o->desc), "Aumenta a taxa de disparo");
        break;
    case UPGRADE_SPEED:
        snprintf(o->label, sizeof(o->label), "Velocidade");
        snprintf(o->desc, sizeof(o->desc), "Aumenta a velocidade de movimento");
        break;
    case UPGRADE_HEAL:
        snprintf(o->label, sizeof(o->label), "Aumento de Vida");
        snprintf(o->desc, sizeof(o->desc), "Recupera e adiciona mais 50 de vida");
        break;
    case UPGRADE_TIME:
        snprintf(o->label, sizeof(o->label), "Aumento de Tempo");
        snprintf(o->desc, sizeof(o->desc), "Acrescenta 20 segundos ao timer");
        break;
    case UPGRADE_GUIDANCE_PP:
        snprintf(o->label, sizeof(o->label), "Balas Guiadas");
        snprintf(o->desc, sizeof(o->desc), "Desbloqueia projeteis guiados com 5 balas guiadas");
        break;
    case UPGRADE_GUIDANCE_APNG:
        snprintf(o->label, sizeof(o->label), "Balas Guiadas+");
        snprintf(o->desc, sizeof(o->desc), "Melhora os projeteis guiados e adiciona mais 5 balas guiadas");
        break;
    case UPGRADE_AMMO:
        snprintf(o->label, sizeof(o->label), "Aumento de Municao");
        snprintf(o->desc, sizeof(o->desc), "Adiciona mais 10 balas guiadas");
        break;
    default:
        snprintf(o->label, sizeof(o->label), "Desconhecido");
        snprintf(o->desc, sizeof(o->desc), "Efeito desconhecido");
        break;
    }
}

void melhorias_rolar_opcoes(Game *g)
{
    int i;
    int used[UPGRADE_COUNT] = {0};

    if (g->player.hasPP)
        used[UPGRADE_GUIDANCE_PP] = 1;
    if (g->player.hasAPNG)
        used[UPGRADE_GUIDANCE_APNG] = 1;

    for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
    {
        UpgradeType t;
        int guard = 0;
        do
        {
            t = (UpgradeType)(rand() % UPGRADE_COUNT);
            if (t == UPGRADE_GUIDANCE_APNG && !g->player.hasPP)
                t = UPGRADE_DAMAGE;
            if ((t == UPGRADE_AMMO) && (!g->player.hasPP && !g->player.hasAPNG))
            {
                t = UPGRADE_DAMAGE;
            }
            guard++;
        } while (used[t] && guard < 32);
        used[t] = 1;
        melhorias_preencher_opcoes(&g->upgrades[i], t);
    }
}

void melhorias_aplicar(Game *g, UpgradeType t)
{
    switch (t)
    {
    case UPGRADE_DAMAGE:
        g->player.damage += 7.5f;
        break;
    case UPGRADE_FIRE_RATE:
        g->player.fireRate *= 0.8f;
        if (g->player.fireRate < 0.1f)
        {
            g->player.fireRate = 0.1f;
        }
        break;
    case UPGRADE_SPEED:
        g->player.speed += 20.0f;
        break;
    case UPGRADE_HEAL:
        g->player.maxHp += 50.0f;
        g->player.hp = g->player.maxHp;
        break;
    case UPGRADE_TIME:
        g->timeLeft += 20.0f;
        break;
    case UPGRADE_GUIDANCE_PP:
        g->player.hasPP = 1;
        g->player.maxGuidedAmmo += 5;
        g->player.guidedAmmo = g->player.maxGuidedAmmo;
        if (g->player.maxLatAccel < 100.0f)
            g->player.maxLatAccel = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case UPGRADE_GUIDANCE_APNG:
        if (!g->player.hasPP)
        {
            g->player.hasPP = 1;
            g->player.maxGuidedAmmo += 10;
        }
        g->player.hasAPNG = 1;
        g->player.maxGuidedAmmo += 5;
        g->player.guidedAmmo = g->player.maxGuidedAmmo;
        if (g->player.maxLatAccel < 100.0f)
            g->player.maxLatAccel = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case UPGRADE_AMMO:
        g->player.maxGuidedAmmo += 10;
        g->player.guidedAmmo = g->player.maxGuidedAmmo;
        break;
    default:
        break;
    }
}

void melhorias_escolher(Game *g, int idx)
{
    if (idx < 0 || idx >= MAXIMO_OPCOES_UPGRADE)
    {
        return;
    }

    melhorias_aplicar(g, g->upgrades[idx].type);
    snprintf(g->lastUpgrade, sizeof(g->lastUpgrade), "%s", g->upgrades[idx].label);
    g->lastUpgradeTimer = 2.5f;
    g->upgradeFlash = 1.0f;
    audio_tocar_som_tiro_disparo();

    g->player.guidedAmmo = g->player.maxGuidedAmmo;

    g->wave += 1;
    cenario_criar_onda(g);
    g->screen = SCREEN_PLAYING;
}
