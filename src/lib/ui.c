#include "ui.h"
#include "enemy.h"
#include "render.h"
#include "persistence.h"
#include "audio.h"
#include "maths.h"

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void ui_draw_player(Game *g)
{
    Vec2 p = g->player.pos;
    float size = g->player.size;

    render_circle(maths_vec2(p.x, p.y - size * 0.4f), size * 0.35f, (Color){0.95f, 0.85f, 0.70f, 1.0f}, 12);

    render_circle(maths_vec2(p.x - size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    render_circle(maths_vec2(p.x + size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);

    render_rect(p.x - size * 0.22f, p.y - size * 0.08f, size * 0.44f, size * 0.35f, (Color){0.2f, 0.3f, 0.7f, 1.0f});

    render_rect(p.x - size * 0.35f, p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    render_rect(p.x + size * 0.1f, p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});

    render_rect(p.x - size * 0.12f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});
    render_rect(p.x + size * 0.07f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});

    if (!g->player.isOnGround)
    {
        render_circle(p, size * 0.9f, (Color){0.3f, 0.9f, 1.0f, 0.2f}, 16);
    }
}

void ui_draw_enemy(Enemy *e)
{
    Vec2 p = enemy_position(e);
    if (e->isBoss)
    {
        Color base = (Color){0.95f, 0.15f + e->hitFlash * 0.45f, 0.85f, 1.0f};
        render_circle(p, e->size, base, 24);
        render_star(p, e->size * 0.72f, (Color){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        render_circle(p, e->size * 1.8f, (Color){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (e->type == ENEMY_TANK)
    {
        render_circle(p, e->size, (Color){0.98f, 0.30f + e->hitFlash * 0.25f, 0.18f, 1.0f}, 20);
        render_rect(p.x - e->size * 0.55f, p.y - e->size * 0.55f, e->size * 1.1f, e->size * 1.1f, (Color){1.0f, 0.76f, 0.18f, 0.75f});
        render_circle(p, e->size * 0.35f, (Color){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (e->type == ENEMY_SNIPER)
    {
        render_diamond(p, e->size * 1.05f, (Color){1.0f, 0.62f + e->hitFlash * 0.25f, 0.24f, 1.0f});
        render_triangle(maths_vec2(p.x, p.y - e->size * 0.1f), e->size * 0.5f, (Color){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (e->type == ENEMY_DIAMOND)
    {
        float s = e->size * 0.7f;
        render_circle(p, e->size, (Color){1.0f, 0.45f + e->hitFlash * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        render_rect(-s, -s, s * 2.0f, s * 2.0f, (Color){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        render_circle(p, e->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (e->type == ENEMY_PENTAGON)
    {
        render_pentagon(p, e->size, (Color){0.2f, 0.9f, 1.0f, 1.0f});
        render_circle(p, e->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        render_circle(p, e->size, (Color){1.0f, 0.35f + e->hitFlash * 0.4f, 0.20f + e->hitFlash * 0.4f, 1.0f}, 20);
        render_star(p, e->size * 0.55f, (Color){1.0f, 0.95f, 0.35f, 0.9f});
    }
    render_circle(e->center, 2.0f, (Color){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void ui_draw_boss_hp_bar(Game *g)
{
    int i;
    for (i = 0; i < MAX_ENEMIES; ++i)
    {
        Enemy *e = &g->enemies[i];
        if (e->active && e->isBoss)
        {
            float pct = e->hp / e->maxHp;
            if (pct < 0.0f)
                pct = 0.0f;
            render_rect(g->width * 0.5f - 250.0f, 20.0f, 500.0f, 16.0f, (Color){0.15f, 0.12f, 0.2f, 0.92f});
            render_rect(g->width * 0.5f - 250.0f, 20.0f, 500.0f * pct, 16.0f, (Color){0.95f, 0.2f, 0.85f, 0.95f});
            render_text(g->width * 0.5f - 56.0f, 42.0f, "MINI BOSS", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.85f, 1.0f);
            break;
        }
    }
}

void ui_draw_hud(Game *g)
{
    char line[128];

    render_rect(15.0f, 52.0f, 450.0f, 62.0f, (Color){0.0f, 0.0f, 0.0f, 0.35f});
    snprintf(line, sizeof(line), "HP: %.0f/%.0f   Time: %.0f   Score: %d   Wave: %d   Gold: %d", g->player.hp, g->player.maxHp, g->timeLeft, g->score, g->wave, g->gold);
    render_text(26.0f, 88.0f, line, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);

    render_rect(26.0f, 108.0f, 180.0f, 10.0f, (Color){0.2f, 0.2f, 0.2f, 0.9f});
    render_rect(26.0f, 108.0f, 180.0f * (g->player.hp / g->player.maxHp), 10.0f, (Color){0.2f, 0.85f, 0.35f, 0.9f});

    if (g->player.hasPP || g->player.hasAPNG)
    {
        char ammo[32];
        snprintf(ammo, sizeof(ammo), "Missiles: %d/%d", g->player.guidedAmmo, g->player.maxGuidedAmmo);
        render_text(220.0f, 118.0f, ammo, GLUT_BITMAP_HELVETICA_12, 1.0f, 0.6f, 0.2f);
    }

    if (g->lastUpgradeTimer > 0.0f)
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "Upgrade: %s", g->lastUpgrade);
        render_rect(g->width - 280.0f, 68.0f, 250.0f, 36.0f, (Color){0.1f, 0.2f, 0.35f, 0.6f});
        render_text(g->width - 266.0f, 90.0f, msg, GLUT_BITMAP_HELVETICA_18, 0.9f, 0.98f, 1.0f);
    }

    if (g->toastTimer > 0.0f && g->toastMessage[0] != '\0')
    {
        render_rect(g->width - 320.0f, 26.0f, 290.0f, 34.0f, (Color){0.06f, 0.12f, 0.25f, 0.72f});
        render_text(g->width - 306.0f, 47.0f, g->toastMessage, GLUT_BITMAP_HELVETICA_12, 0.92f, 0.98f, 1.0f);
    }
}

void ui_draw_crosshair(Game *g)
{
    Vec2 m = enemy_mouse_to_world(g);
    glColor4f(0.9f, 1.0f, 1.0f, 0.9f);
    glBegin(GL_LINES);
    glVertex2f(m.x - 10.0f, m.y);
    glVertex2f(m.x + 10.0f, m.y);
    glVertex2f(m.x, m.y - 10.0f);
    glVertex2f(m.x, m.y + 10.0f);
    glEnd();
    render_circle(m, 4.0f, (Color){0.3f, 0.9f, 1.0f, 0.65f}, 12);
}

void ui_refresh_top_scores(Game *g)
{
    g->topScoreCount = persistence_load_top_scores(g->topScores, 5);
}

void ui_refresh_all_scores(Game *g)
{
    g->allScoreCount = persistence_load_scores(g->allScores, 64);
    if (g->scorePage < 0)
    {
        g->scorePage = 0;
    }
}

void ui_set_toast(Game *g, const char *msg)
{
    snprintf(g->toastMessage, sizeof(g->toastMessage), "%s", msg ? msg : "");
    g->toastTimer = 2.2f;
}

void ui_set_end_screen(Game *g, GameScreen end)
{
    audio_stop_bgm();
    g->screen = end;
    g->enteringName = 1;
    g->nameSaved = 0;

    if (g->score > g->highScore)
    {
        g->highScore = g->score;
    }
    if (g->wave > g->maxWaveEver)
    {
        g->maxWaveEver = g->wave;
    }
    persistence_save_stats(g->highScore, g->maxWaveEver);
    ui_refresh_top_scores(g);
}
