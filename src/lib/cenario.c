#include "cenario.h"
#include "inimigo.h"
#include "matematica.h"

#include <string.h>

void cenario_limpar_entidades(Game *g)
{
    memset(g->enemies, 0, sizeof(g->enemies));
    memset(g->projectiles, 0, sizeof(g->projectiles));
    memset(g->particles, 0, sizeof(g->particles));
}

void cenario_limpar_plataformas(Game *g)
{
    memset(g->obstacles, 0, sizeof(g->obstacles));
}

void cenario_criar_plataformas(Game *g)
{
    int count;
    int i, j;
    int validPlacement;
    int attempts;

    cenario_limpar_plataformas(g);
    count = 2 + g->wave / 2;
    if (g->difficulty >= 2)
    {
        count += 1;
    }
    if (count > MAXIMO_PLATAFORMAS)
    {
        count = MAXIMO_PLATAFORMAS;
    }

    for (i = 0; i < count; ++i)
    {
        Obstacle *o = &g->obstacles[i];

        attempts = 0;
        do
        {
            validPlacement = 1;
            o->active = 1;
            o->w = matematica_float_aleatorio(100.0f, 220.0f);
            o->h = matematica_float_aleatorio(22.0f, 44.0f);
            o->x = matematica_float_aleatorio(40.0f, (float)g->width - o->w - 40.0f);
            o->y = matematica_float_aleatorio(ALTURA_CHAO - 200.0f, ALTURA_CHAO - 150.0f);

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

void cenario_criar_onda(Game *g)
{
    int quantidade_inimigos = 3 + g->wave * 1.2;
    int i;
    int finalWave = (g->wave >= g->wavesToWin);
    float hpMul = inimigo_multiplicador_vida(g->difficulty);
    float dmgMul = inimigo_multiplicador_dano(g->difficulty);
    float fireMul = inimigo_taxa_disparo(g->difficulty);

    if (finalWave)
    {
        quantidade_inimigos = 1 + g->wave;
    }

    if (quantidade_inimigos > MAXIMO_INIMIGOS)
    {
        quantidade_inimigos = MAXIMO_INIMIGOS;
    }

    cenario_limpar_entidades(g);
    cenario_criar_plataformas(g);
    g->enemiesRemaining = quantidade_inimigos;

    for (i = 0; i < quantidade_inimigos; ++i)
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
                        ? matematica_vetor2d((float)g->width * 0.5f, (float)g->height * 0.22f)
                        : matematica_vetor2d(matematica_float_aleatorio(120.0f, (float)g->width - 120.0f), matematica_float_aleatorio((float)g->height * 0.14f, (float)g->height * 0.44f));
        e->orbitRadius = e->isBoss ? 96.0f : matematica_float_aleatorio(24.0f, 70.0f);
        e->angle = matematica_float_aleatorio(0.0f, 2.0f * (float)M_PI);
        e->angularSpeed = (e->isBoss ? matematica_float_aleatorio(0.45f, 0.85f) : matematica_float_aleatorio(0.75f, 1.6f)) * ((i % 2 == 0) ? 1.0f : -1.0f) * (1.0f + g->wave * 0.08f);

        if (e->isBoss)
        {
            e->size = 42.0f;
            e->hp = 420.0f + g->wave * 45.0f;
            e->damage = 24.0f + g->wave * 2.7f;
            e->shootCooldown = matematica_float_aleatorio(0.4f, 1.0f);
        }
        else if (e->type == ENEMY_TANK)
        {
            e->size = matematica_float_aleatorio(24.0f, 30.0f);
            e->hp = 50.0f + g->wave * 14.0f;
            e->damage = 18.0f + g->wave * 2.6f;
            e->shootCooldown = matematica_float_aleatorio(1.3f, 2.8f);
            e->angularSpeed *= 0.72f;
        }
        else if (e->type == ENEMY_SNIPER)
        {
            e->size = matematica_float_aleatorio(14.0f, 18.0f);
            e->hp = 20.0f + g->wave * 7.5f;
            e->damage = 14.0f + g->wave * 2.0f;
            e->shootCooldown = matematica_float_aleatorio(0.6f, 1.4f);
            e->angularSpeed *= 1.2f;
            e->orbitRadius += 18.0f;
        }
        else if (e->type == ENEMY_DIAMOND)
        {
            e->size = matematica_float_aleatorio(18.0f, 22.0f);
            e->hp = 35.0f + g->wave * 10.0f;
            e->damage = 15.0f + g->wave * 2.0f;
            e->shootCooldown = matematica_float_aleatorio(2.0f, 4.0f);
            e->burstCount = 2;
        }
        else if (e->type == ENEMY_PENTAGON)
        {
            e->size = matematica_float_aleatorio(20.0f, 25.0f);
            e->hp = 45.0f + g->wave * 12.0f;
            e->damage = 20.0f + g->wave * 2.5f;
            e->shootCooldown = matematica_float_aleatorio(3.0f, 5.0f);
        }
        else
        {
            e->size = matematica_float_aleatorio(16.0f, 24.0f);
            e->hp = 26.0f + g->wave * 9.0f;
            e->damage = 16.0f + g->wave * 2.2f;
            e->shootCooldown = matematica_float_aleatorio(0.8f, 2.2f);
        }

        e->maxHp = e->hp;
        e->hp *= hpMul;
        e->maxHp = e->hp;
        e->damage *= dmgMul;
        e->shootCooldown *= fireMul;
        e->hitFlash = 0.0f;

        e->vel = matematica_vetor2d(0.0f, 0.0f);
        e->accel = matematica_vetor2d(0.0f, 0.0f);
    }
}
