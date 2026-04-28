#include "world.h"
#include "enemy.h"
#include "maths.h"
#include "render.h"

#include <GL/glut.h>
#include <math.h>
#include <string.h>

void world_clear_entities(Game *g)
{
    memset(g->enemies, 0, sizeof(g->enemies));
    memset(g->projectiles, 0, sizeof(g->projectiles));
    memset(g->particles, 0, sizeof(g->particles));
}

void world_clear_obstacles(Game *g)
{
    memset(g->obstacles, 0, sizeof(g->obstacles));
}

void world_setup_obstacles(Game *g)
{
    int count;
    int i, j;
    int validPlacement;
    int attempts;

    world_clear_obstacles(g);
    count = 2 + g->wave / 2;
    if (g->difficulty >= 2)
    {
        count += 1;
    }
    if (count > MAX_OBSTACLES)
    {
        count = MAX_OBSTACLES;
    }

    for (i = 0; i < count; ++i)
    {
        Obstacle *o = &g->obstacles[i];

        attempts = 0;
        do
        {
            validPlacement = 1;
            o->active = 1;
            o->w = maths_randf(100.0f, 220.0f);
            o->h = maths_randf(22.0f, 44.0f);
            o->x = maths_randf(40.0f, (float)g->width - o->w - 40.0f);
            o->y = maths_randf((float)g->height * 0.45f, (float)g->height * 0.65f);

            for (j = 0; j < i; ++j)
            {
                Obstacle *other = &g->obstacles[j];
                if (!other->active)
                    continue;

                if (!(o->x + o->w + 20.0f < other->x || o->x > other->x + other->w + 20.0f ||
                      o->y + o->h + 20.0f < other->y || o->y > other->y + other->h + 20.0f))
                {
                    validPlacement = 0;
                    break;
                }
            }

            attempts++;
        } while (!validPlacement && attempts < 10);

        if (!validPlacement)
        {
            o->active = 0;
        }
    }
}

void world_spawn_wave(Game *g)
{
    int amount = 4 + g->wave * 2;
    int i;
    int finalWave = (g->wave >= g->wavesToWin);
    float hpMul = enemy_hp_mul(g->difficulty);
    float dmgMul = enemy_damage_mul(g->difficulty);
    float fireMul = enemy_fire_rate_mul(g->difficulty);

    if (finalWave)
    {
        amount = 1 + g->wave;
    }

    if (amount > MAX_ENEMIES)
    {
        amount = MAX_ENEMIES;
    }

    world_clear_entities(g);
    world_setup_obstacles(g);
    g->enemiesRemaining = amount;

    for (i = 0; i < amount; ++i)
    {
        Enemy *e = &g->enemies[i];
        e->active = 1;
        e->isBoss = (finalWave && i == 0);
        if (e->isBoss)
        {
            e->type = ENEMY_TANK;
        }
        else if (g->wave >= 3 && (i % 6 == 0))
        {
            e->type = ENEMY_PENTAGON;
        }
        else if (g->wave >= 2 && (i % 5 == 0))
        {
            e->type = ENEMY_DIAMOND;
        }
        else if (g->wave >= 4 && (i % 7 == 0))
        {
            e->type = ENEMY_TANK;
        }
        else if (g->wave >= 1 && (i % 4 == 0))
        {
            e->type = ENEMY_SNIPER;
        }
        else
        {
            e->type = ENEMY_STANDARD;
        }

        e->center = e->isBoss
                        ? maths_vec2((float)g->width * 0.5f, (float)g->height * 0.22f)
                        : maths_vec2(maths_randf(120.0f, (float)g->width - 120.0f), maths_randf((float)g->height * 0.14f, (float)g->height * 0.44f));
        e->orbitRadius = e->isBoss ? 96.0f : maths_randf(24.0f, 70.0f);
        e->angle = maths_randf(0.0f, 2.0f * (float)M_PI);
        e->angularSpeed = (e->isBoss ? maths_randf(0.45f, 0.85f) : maths_randf(0.75f, 1.6f)) * ((i % 2 == 0) ? 1.0f : -1.0f) * (1.0f + g->wave * 0.08f);

        if (e->isBoss)
        {
            e->size = 42.0f;
            e->hp = 420.0f + g->wave * 45.0f;
            e->damage = 24.0f + g->wave * 2.7f;
            e->shootCooldown = maths_randf(0.4f, 1.0f);
        }
        else if (e->type == ENEMY_TANK)
        {
            e->size = maths_randf(24.0f, 30.0f);
            e->hp = 50.0f + g->wave * 14.0f;
            e->damage = 18.0f + g->wave * 2.6f;
            e->shootCooldown = maths_randf(1.3f, 2.8f);
            e->angularSpeed *= 0.72f;
        }
        else if (e->type == ENEMY_SNIPER)
        {
            e->size = maths_randf(14.0f, 18.0f);
            e->hp = 20.0f + g->wave * 7.5f;
            e->damage = 14.0f + g->wave * 2.0f;
            e->shootCooldown = maths_randf(0.6f, 1.4f);
            e->angularSpeed *= 1.2f;
            e->orbitRadius += 18.0f;
        }
        else if (e->type == ENEMY_DIAMOND)
        {
            e->size = maths_randf(18.0f, 22.0f);
            e->hp = 35.0f + g->wave * 10.0f;
            e->damage = 15.0f + g->wave * 2.0f;
            e->shootCooldown = maths_randf(2.0f, 4.0f);
            e->burstCount = 3;
        }
        else if (e->type == ENEMY_PENTAGON)
        {
            e->size = maths_randf(20.0f, 25.0f);
            e->hp = 45.0f + g->wave * 12.0f;
            e->damage = 20.0f + g->wave * 2.5f;
            e->shootCooldown = maths_randf(3.0f, 5.0f);
        }
        else
        {
            e->size = maths_randf(16.0f, 24.0f);
            e->hp = 26.0f + g->wave * 9.0f;
            e->damage = 16.0f + g->wave * 2.2f;
            e->shootCooldown = maths_randf(0.8f, 2.2f);
        }

        e->maxHp = e->hp;
        e->hp *= hpMul;
        e->maxHp = e->hp;
        e->damage *= dmgMul;
        e->shootCooldown *= fireMul;
        e->hitFlash = 0.0f;

        e->vel = maths_vec2(0.0f, 0.0f);
        e->accel = maths_vec2(0.0f, 0.0f);
    }
}

void world_draw_obstacles(Game *g)
{
    int i;
    for (i = 0; i < MAX_OBSTACLES; ++i)
    {
        Obstacle *o = &g->obstacles[i];
        if (!o->active)
        {
            continue;
        }
        render_rect(o->x, o->y, o->w, o->h, (Color){0.22f, 0.28f, 0.38f, 0.82f});
        render_rect(o->x + 3.0f, o->y + 3.0f, o->w - 6.0f, o->h - 6.0f, (Color){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void world_draw_background(Game *g)
{
    float offset = fmodf(g->elapsed * 20.0f, (float)g->width);

    if (g->bgTextureLoaded)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g->bgTexture);
        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + offset / g->width, 0.0f);
        glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + offset / g->width, 0.0f);
        glVertex2f((float)g->width, 0.0f);
        glTexCoord2f(1.0f + offset / g->width, 1.0f);
        glVertex2f((float)g->width, (float)g->height);
        glTexCoord2f(0.0f + offset / g->width, 1.0f);
        glVertex2f(0.0f, (float)g->height);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    render_rect(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.04f, 0.06f, 0.11f, 1.0f});

    render_circle(maths_vec2(g->width * 0.85f, g->height * 0.25f), 60.0f, (Color){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    render_circle(maths_vec2(g->width * 0.88f, g->height * 0.22f), 58.0f, (Color){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    render_rect(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int i;
        for (i = 0; i < 18; ++i)
        {
            float x = fmodf((i * 97.0f + g->elapsed * (8.0f + i)), (float)g->width);
            float y = 80.0f + fmodf((i * 61.0f + g->elapsed * (5.0f + i * 0.25f)), (float)g->height - 100.0f);
            render_circle(maths_vec2(x, y), 2.0f + (i % 3), (Color){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    render_rect(0.0f, GROUND_Y, (float)g->width, (float)g->height - GROUND_Y, (Color){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int i;
        for (i = 0; i < 20; ++i)
        {
            float x1 = i * (g->width / 20.0f);
            render_rect(x1, GROUND_Y - 4.0f, (g->width / 40.0f), 3.0f, (Color){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}
