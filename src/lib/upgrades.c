#include "upgrades.h"
#include "maths.h"
#include "render.h"
#include "world.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

void upgrades_fill_option(UpgradeOption *o, UpgradeType type)
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

void upgrades_roll(Game *g)
{
    int i;
    int used[UPGRADE_COUNT] = {0};

    if (g->player.hasPP)
        used[UPGRADE_GUIDANCE_PP] = 1;
    if (g->player.hasAPNG)
        used[UPGRADE_GUIDANCE_APNG] = 1;

    for (i = 0; i < MAX_UPGRADE_OPTIONS; ++i)
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
        upgrades_fill_option(&g->upgrades[i], t);
    }
}

void upgrades_apply(Game *g, UpgradeType t)
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
            g->player.maxLatAccel = MAX_PLAYER_LAT_ACCEL;
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
            g->player.maxLatAccel = MAX_PLAYER_LAT_ACCEL;
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

void upgrades_draw_icon(UpgradeType t, float x, float y)
{
    switch (t)
    {
    case UPGRADE_DAMAGE:
        render_star(maths_vec2(x, y), 16.0f, (Color){1.0f, 0.7f, 0.25f, 0.95f});
        break;
    case UPGRADE_FIRE_RATE:
        render_triangle(maths_vec2(x, y), 14.0f, (Color){1.0f, 0.95f, 0.5f, 0.95f});
        render_triangle(maths_vec2(x + 10.0f, y), 10.0f, (Color){1.0f, 0.75f, 0.2f, 0.85f});
        break;
    case UPGRADE_SPEED:
        render_diamond(maths_vec2(x - 6.0f, y), 10.0f, (Color){0.5f, 1.0f, 0.9f, 0.95f});
        render_diamond(maths_vec2(x + 8.0f, y), 8.0f, (Color){0.3f, 0.9f, 1.0f, 0.9f});
        break;
    case UPGRADE_HEAL:
        render_rect(x - 4.0f, y - 12.0f, 8.0f, 24.0f, (Color){0.45f, 1.0f, 0.45f, 0.95f});
        render_rect(x - 12.0f, y - 4.0f, 24.0f, 8.0f, (Color){0.45f, 1.0f, 0.45f, 0.95f});
        break;
    case UPGRADE_TIME:
        render_circle(maths_vec2(x, y), 13.0f, (Color){0.7f, 0.85f, 1.0f, 0.95f}, 16);
        render_rect(x - 1.0f, y - 1.0f, 2.0f, 9.0f, (Color){0.1f, 0.2f, 0.5f, 0.95f});
        render_rect(x - 1.0f, y - 1.0f, 7.0f, 2.0f, (Color){0.1f, 0.2f, 0.5f, 0.95f});
        break;
    case UPGRADE_GUIDANCE_PP:
        render_circle(maths_vec2(x, y), 10.0f, (Color){0.2f, 0.9f, 1.0f, 0.9f}, 16);
        break;
    case UPGRADE_GUIDANCE_APNG:
        render_circle(maths_vec2(x, y), 10.0f, (Color){0.9f, 0.2f, 1.0f, 0.9f}, 16);
        break;
    default:
        render_circle(maths_vec2(x, y), 12.0f, (Color){1.0f, 1.0f, 1.0f, 0.8f}, 12);
        break;
    }
}

void upgrades_choose(Game *g, int idx)
{
    if (idx < 0 || idx >= MAX_UPGRADE_OPTIONS)
    {
        return;
    }

    upgrades_apply(g, g->upgrades[idx].type);
    snprintf(g->lastUpgrade, sizeof(g->lastUpgrade), "%s", g->upgrades[idx].label);
    g->lastUpgradeTimer = 2.5f;
    g->upgradeFlash = 1.0f;
    audio_play_shoot();

    g->player.guidedAmmo = g->player.maxGuidedAmmo;

    g->wave += 1;
    world_spawn_wave(g);
    g->screen = SCREEN_PLAYING;
}
