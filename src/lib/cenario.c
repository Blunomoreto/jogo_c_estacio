#include "cenario.h"
#include "inimigo.h"
#include "matematica.h"

#include <string.h>

void cenario_limpar_entidades(Game *jogo)
{
    memset(jogo->enemies, 0, sizeof(jogo->enemies));
    memset(jogo->projectiles, 0, sizeof(jogo->projectiles));
    memset(jogo->particles, 0, sizeof(jogo->particles));
}

void cenario_limpar_plataformas(Game *jogo)
{
    memset(jogo->obstacles, 0, sizeof(jogo->obstacles));
}

void cenario_criar_plataformas(Game *jogo)
{
    int count;
    int i, j;
    int validPlacement;
    int attempts;

    cenario_limpar_plataformas(jogo);
    count = 2 + jogo->wave / 2;
    if (jogo->difficulty >= 2)
    {
        count += 1;
    }
    if (count > MAXIMO_PLATAFORMAS)
    {
        count = MAXIMO_PLATAFORMAS;
    }

    for (i = 0; i < count; ++i)
    {
        Obstacle *o = &jogo->obstacles[i];

        attempts = 0;
        do
        {
            validPlacement = 1;
            o->active = 1;
            o->w = matematica_float_aleatorio_alcance(100.0f, 220.0f);
            o->h = matematica_float_aleatorio_alcance(22.0f, 44.0f);
            o->x = matematica_float_aleatorio_alcance(40.0f, (float)jogo->width - o->w - 40.0f);
            o->y = matematica_float_aleatorio_alcance(ALTURA_CHAO - 200.0f, ALTURA_CHAO - 150.0f);

            for (j = 0; j < i; ++j)
            {
                Obstacle *other = &jogo->obstacles[j];
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

void cenario_criar_onda(Game *jogo)
{
    int quantidade_inimigos = 3 + jogo->wave * 1.2;
    int i;
    int finalWave = (jogo->wave >= jogo->wavesToWin);
    float hpMul = inimigo_multiplicador_vida(jogo->difficulty);
    float dmgMul = inimigo_multiplicador_dano(jogo->difficulty);
    float fireMul = inimigo_taxa_disparo(jogo->difficulty);

    if (finalWave)
    {
        quantidade_inimigos = 1 + jogo->wave;
    }

    if (quantidade_inimigos > MAXIMO_INIMIGOS)
    {
        quantidade_inimigos = MAXIMO_INIMIGOS;
    }

    cenario_limpar_entidades(jogo);
    cenario_criar_plataformas(jogo);
    jogo->enemiesRemaining = quantidade_inimigos;

    for (i = 0; i < quantidade_inimigos; ++i)
    {
        Enemy *e = &jogo->enemies[i];
        e->active = 1;
        e->isBoss = (finalWave && i == 0);
        if (e->isBoss)
        {
            e->type = ENEMY_TANK;
        }
        else if (jogo->wave >= 3 && (i % 6 == 0))
        {
            e->type = ENEMY_PENTAGON;
        }
        else if (jogo->wave >= 2 && (i % 5 == 0))
        {
            e->type = ENEMY_DIAMOND;
        }
        else if (jogo->wave >= 4 && (i % 7 == 0))
        {
            e->type = ENEMY_TANK;
        }
        else if (jogo->wave >= 1 && (i % 4 == 0))
        {
            e->type = ENEMY_SNIPER;
        }
        else
        {
            e->type = ENEMY_STANDARD;
        }

        e->center = e->isBoss
                        ? matematica_vetor2d((float)jogo->width * 0.5f, (float)jogo->height * 0.22f)
                        : matematica_vetor2d(matematica_float_aleatorio_alcance(120.0f, (float)jogo->width - 120.0f), matematica_float_aleatorio_alcance((float)jogo->height * 0.14f, (float)jogo->height * 0.44f));
        e->orbitRadius = e->isBoss ? 96.0f : matematica_float_aleatorio_alcance(24.0f, 70.0f);
        e->angle = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
        e->angularSpeed = (e->isBoss ? matematica_float_aleatorio_alcance(0.45f, 0.85f) : matematica_float_aleatorio_alcance(0.75f, 1.6f)) * ((i % 2 == 0) ? 1.0f : -1.0f) * (1.0f + jogo->wave * 0.08f);

        if (e->isBoss)
        {
            e->size = 42.0f;
            e->hp = 420.0f + jogo->wave * 45.0f;
            e->damage = 24.0f + jogo->wave * 2.7f;
            e->shootCooldown = matematica_float_aleatorio_alcance(0.4f, 1.0f);
        }
        else if (e->type == ENEMY_TANK)
        {
            e->size = matematica_float_aleatorio_alcance(24.0f, 30.0f);
            e->hp = 50.0f + jogo->wave * 14.0f;
            e->damage = 18.0f + jogo->wave * 2.6f;
            e->shootCooldown = matematica_float_aleatorio_alcance(1.3f, 2.8f);
            e->angularSpeed *= 0.72f;
        }
        else if (e->type == ENEMY_SNIPER)
        {
            e->size = matematica_float_aleatorio_alcance(14.0f, 18.0f);
            e->hp = 20.0f + jogo->wave * 7.5f;
            e->damage = 14.0f + jogo->wave * 2.0f;
            e->shootCooldown = matematica_float_aleatorio_alcance(0.6f, 1.4f);
            e->angularSpeed *= 1.2f;
            e->orbitRadius += 18.0f;
        }
        else if (e->type == ENEMY_DIAMOND)
        {
            e->size = matematica_float_aleatorio_alcance(18.0f, 22.0f);
            e->hp = 35.0f + jogo->wave * 10.0f;
            e->damage = 15.0f + jogo->wave * 2.0f;
            e->shootCooldown = matematica_float_aleatorio_alcance(2.0f, 4.0f);
            e->burstCount = 2;
        }
        else if (e->type == ENEMY_PENTAGON)
        {
            e->size = matematica_float_aleatorio_alcance(20.0f, 25.0f);
            e->hp = 45.0f + jogo->wave * 12.0f;
            e->damage = 20.0f + jogo->wave * 2.5f;
            e->shootCooldown = matematica_float_aleatorio_alcance(3.0f, 5.0f);
        }
        else
        {
            e->size = matematica_float_aleatorio_alcance(16.0f, 24.0f);
            e->hp = 26.0f + jogo->wave * 9.0f;
            e->damage = 16.0f + jogo->wave * 2.2f;
            e->shootCooldown = matematica_float_aleatorio_alcance(0.8f, 2.2f);
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
