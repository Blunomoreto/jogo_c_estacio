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
        snprintf(o->label, sizeof(o->label), "+Damage");
        snprintf(o->desc, sizeof(o->desc), "Increase projectile damage by +6");
        break;
    case UPGRADE_FIRE_RATE:
        snprintf(o->label, sizeof(o->label), "+Fire Rate");
        snprintf(o->desc, sizeof(o->desc), "Shoot faster (cooldown -18%%)");
        break;
    case UPGRADE_SPEED:
        snprintf(o->label, sizeof(o->label), "+Speed");
        snprintf(o->desc, sizeof(o->desc), "Movement speed +45");
        break;
    case UPGRADE_HEAL:
        snprintf(o->label, sizeof(o->label), "Heal");
        snprintf(o->desc, sizeof(o->desc), "Recover 26 HP");
        break;
    case UPGRADE_TIME:
        snprintf(o->label, sizeof(o->label), "+Time");
        snprintf(o->desc, sizeof(o->desc), "Add 18 seconds to timer");
        break;
    case UPGRADE_GUIDANCE_PP:
        snprintf(o->label, sizeof(o->label), "Guidance PP");
        snprintf(o->desc, sizeof(o->desc), "Unlock Pure Pursuit for right click. 10 missiles start.");
        break;
    case UPGRADE_GUIDANCE_APNG:
        snprintf(o->label, sizeof(o->label), "Guidance APN");
        snprintf(o->desc, sizeof(o->desc), "Unlock Augmented Proportional Navigation for right click. 10 missiles start.");
        break;
    case UPGRADE_AMMO:
        snprintf(o->label, sizeof(o->label), "Ammo Pack");
        snprintf(o->desc, sizeof(o->desc), "Increase max guided missiles by +8");
        break;
    case UPGRADE_OVERLOAD:
        snprintf(o->label, sizeof(o->label), "Overload");
        snprintf(o->desc, sizeof(o->desc), "Increase missile maneuverability (G-limit) by +25%%");
        break;
    default:
        snprintf(o->label, sizeof(o->label), "Unknown");
        snprintf(o->desc, sizeof(o->desc), "Unknown effect");
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
            if ((t == UPGRADE_AMMO || t == UPGRADE_OVERLOAD) && (!g->player.hasPP && !g->player.hasAPNG))
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
        g->player.damage += 6.0f;
        break;
    case UPGRADE_FIRE_RATE:
        g->player.fireRate *= 0.82f;
        if (g->player.fireRate < 0.08f)
        {
            g->player.fireRate = 0.08f;
        }
        break;
    case UPGRADE_SPEED:
        g->player.speed += 45.0f;
        break;
    case UPGRADE_HEAL:
        g->player.hp += 26.0f;
        if (g->player.hp > g->player.maxHp)
        {
            g->player.hp = g->player.maxHp;
        }
        break;
    case UPGRADE_TIME:
        g->timeLeft += 18.0f;
        break;
    case UPGRADE_GUIDANCE_PP:
        g->player.hasPP = 1;
        g->player.maxGuidedAmmo += 10;
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
        g->player.maxGuidedAmmo += 10;
        g->player.guidedAmmo = g->player.maxGuidedAmmo;
        if (g->player.maxLatAccel < 100.0f)
            g->player.maxLatAccel = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case UPGRADE_AMMO:
        g->player.maxGuidedAmmo += 8;
        g->player.guidedAmmo = g->player.maxGuidedAmmo;
        break;
    case UPGRADE_OVERLOAD:
        if (g->player.hasAPNG)
        {
            g->player.maxLatAccel *= 1.25f;
        }
        else
        {
            g->player.hp += 26.0f;
        }
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
