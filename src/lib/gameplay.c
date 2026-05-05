#include "gameplay.h"
#include "jogo.h"
#include "audio.h"
#include "colisao.h"
#include "configuracao.h"
#include "inimigo.h"
#include "interface.h"
#include "matematica.h"
#include "melhorias.h"
#include "particulas.h"
#include "projeteis.h"

#include <GL/glut.h>
#include <math.h>

static void gameplay_atualizar_jogador_movimento(Game *jogo, float delta_tempo)
{
    int oi;
    float moveX = 0.0f;

    if (jogo->input.keys['a'] || jogo->input.keys['A'] || jogo->input.special[GLUT_KEY_LEFT])
        moveX -= 1.0f;
    if (jogo->input.keys['d'] || jogo->input.keys['D'] || jogo->input.special[GLUT_KEY_RIGHT])
        moveX += 1.0f;

    jogo->player.pos.x += moveX * jogo->player.speed * delta_tempo;

    if (jogo->player.pos.x < jogo->player.size)
        jogo->player.pos.x = jogo->player.size;
    if (jogo->player.pos.x > jogo->width - jogo->player.size)
        jogo->player.pos.x = jogo->width - jogo->player.size;

    if (jogo->input.keys[' '] || jogo->input.keys['w'] || jogo->input.keys['W'])
    {
        if (jogo->player.isOnGround)
        {
            jogo->player.velY = -JOGADOR_FORCA_PULO;
            jogo->player.isOnGround = 0;
            audio_tocar_som_pulo_inicio();
        }
    }

    jogo->player.velY += GRAVIDADE * delta_tempo;
    if (jogo->player.velY > 600.0f)
        jogo->player.velY = 600.0f;
    jogo->player.pos.y += jogo->player.velY * delta_tempo;

    {
        static Vetor2D prevP = {0, 0};
        static Vetor2D prevV = {0, 0};
        if (prevP.x == 0 && prevP.y == 0)
            prevP = jogo->player.pos;
        jogo->player.vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->player.pos, prevP), 1.0f / delta_tempo);
        jogo->player.accel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->player.vel, prevV), 1.0f / delta_tempo);
        prevP = jogo->player.pos;
        prevV = jogo->player.vel;
    }

    jogo->player.isOnGround = 0;
    if (jogo->player.pos.y + jogo->player.size >= ALTURA_CHAO)
    {
        if (jogo->player.velY > 100.0f)
            audio_tocar_som_pulo_fim();
        jogo->player.pos.y = ALTURA_CHAO - jogo->player.size;
        jogo->player.velY = 0.0f;
        jogo->player.isOnGround = 1;
    }

    if (jogo->player.pos.y - jogo->player.size <= 20.0f)
    {
        jogo->player.pos.y = 20.0f + jogo->player.size;
        jogo->player.velY = 0.0f;
    }

    for (oi = 0; oi < MAXIMO_PLATAFORMAS; ++oi)
    {
        Obstacle *o = &jogo->obstacles[oi];
        if (!o->active)
            continue;
        if (colisao_circulo_vs_retangulo(jogo->player.pos, jogo->player.size,
                                         matematica_vetor2d(o->x, o->y),
                                         matematica_vetor2d(o->x + o->w, o->y + o->h)))
        {
            float cx = o->x + o->w * 0.5f;
            float cy = o->y + o->h * 0.5f;
            float dx = fabsf(jogo->player.pos.x - cx);
            float dy = fabsf(jogo->player.pos.y - cy);

            if (jogo->player.pos.y - jogo->player.size < cy && jogo->player.velY >= 0.0f)
            {
                if (jogo->player.velY > 50.0f)
                    audio_tocar_som_pulo_fim();
                jogo->player.pos.y = o->y - jogo->player.size;
                jogo->player.velY = 0.0f;
                jogo->player.isOnGround = 1;
            }
            else if (jogo->player.pos.y - jogo->player.size >= cy && dx > dy)
            {
                if (jogo->player.pos.x < cx)
                    jogo->player.pos.x = o->x - jogo->player.size - 5.0f;
                else
                    jogo->player.pos.x = o->x + o->w + jogo->player.size + 5.0f;
                if (jogo->player.pos.x < jogo->player.size)
                    jogo->player.pos.x = jogo->player.size;
                if (jogo->player.pos.x > jogo->width - jogo->player.size)
                    jogo->player.pos.x = jogo->width - jogo->player.size;
            }
        }
    }
}

static void gameplay_atirar_jogador(Game *jogo, float delta_tempo)
{
    int i;
    jogo->player.fireCooldown -= delta_tempo;

    if (jogo->input.mouseDown[0] && jogo->player.fireCooldown <= 0.0f)
    {
        Vetor2D target = matematica_mouse_para_mundo(jogo);
        Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, jogo->player.pos));
        projeteis_criar(jogo, jogo->player.pos, dir, 1, jogo->player.projectileSpeed, jogo->player.damage, 6.0f, 2.5f, GUIDANCE_NONE, -1, 0.0f);
        particulas_criar(jogo, jogo->player.pos, 4, (Color){0.3f, 0.9f, 1.0f, 0.85f});
        jogo->player.fireCooldown = jogo->player.fireRate;
        audio_tocar_som_tiro_disparo();
    }

    if (jogo->input.mouseDown[2] && jogo->player.fireCooldown <= 0.0f && jogo->player.guidedAmmo > 0)
    {
        if (jogo->player.hasPP || jogo->player.hasAPNG)
        {
            Vetor2D target = matematica_mouse_para_mundo(jogo);
            int bestTarget = -1;
            float minD = 1000.0f;
            for (i = 0; i < MAXIMO_INIMIGOS; ++i)
            {
                if (jogo->enemies[i].active)
                {
                    float d = matematica_vetor2d_len(matematica_vetor2d_subtracao(inimigo_posicao(&jogo->enemies[i]), target));
                    if (d < minD)
                    {
                        minD = d;
                        bestTarget = i;
                    }
                }
            }
            if (bestTarget != -1)
            {
                Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, jogo->player.pos));
                GuidanceType law = jogo->player.hasAPNG ? GUIDANCE_APNG : GUIDANCE_PP;
                projeteis_criar(jogo, jogo->player.pos, dir, 1, jogo->player.speed * 1.3f, jogo->player.damage * 3.0f, 8.0f, 5.0f, law, bestTarget, jogo->player.maxLatAccel);
                jogo->player.guidedAmmo--;
                jogo->player.fireCooldown = jogo->player.fireRate * 2.0f;
                audio_tocar_som_tiro_disparo();
            }
        }
    }
}

static void gameplay_atualizar_projeteis_internos(Game *jogo, float delta_tempo)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &jogo->projectiles[i];
        int oi;
        if (!p->active)
            continue;

        if (p->guidance != GUIDANCE_NONE && p->missed)
        {
            p->sdTimer -= delta_tempo;
            if (p->sdTimer <= 0.0f)
                p->active = 0;
            p->life -= delta_tempo;
            p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, delta_tempo));
            continue;
        }

        if (p->guidance != GUIDANCE_NONE)
        {
            float speed = matematica_vetor2d_len(p->vel);
            float gamma = atan2f(p->vel.y, p->vel.x);
            float aCmd = 0.0f;
            Vetor2D targetPos, targetVel = {0, 0}, targetAccel = {0, 0};
            int targetValid = 0;

            if (p->fromPlayer)
            {
                if (p->targetIdx != -1 && jogo->enemies[p->targetIdx].active)
                {
                    Enemy *e = &jogo->enemies[p->targetIdx];
                    targetPos = inimigo_posicao(e);
                    targetVel = e->vel;
                    targetAccel = e->accel;
                    targetValid = 1;
                }
            }
            else
            {
                targetPos = jogo->player.pos;
                targetVel = jogo->player.vel;
                targetAccel = jogo->player.accel;
                targetValid = 1;
            }

            if (targetValid)
            {
                Vetor2D dPos = matematica_vetor2d_subtracao(targetPos, p->pos);
                float dist = matematica_vetor2d_len(dPos);
                float los = atan2f(dPos.y, dPos.x);
                float losRate = (dPos.x * (targetVel.y - p->vel.y) - dPos.y * (targetVel.x - p->vel.x)) / (dist * dist);

                if (p->guidance == GUIDANCE_APNG && p->prevDist < 9999.0f &&
                    dist > p->prevDist && dist < 250.0f && p->prevDist < 270.0f)
                {
                    p->missed = 1;
                    p->sdTimer = 0.2f;
                }

                if (p->guidance == GUIDANCE_PP && !p->missed && p->prevDist < 9999.0f)
                {
                    if (fabs(losRate) * speed > p->maxLatAccel && dist < 300.0f)
                    {
                        p->missed = 1;
                        p->sdTimer = 0.2f;
                    }
                }

                p->prevDist = dist;

                if (!p->missed)
                {
                    if (p->guidance == GUIDANCE_APNG)
                    {
                        Vetor2D relV = matematica_vetor2d_subtracao(targetVel, p->vel);
                        float Vc = -(dPos.x * relV.x + dPos.y * relV.y) / dist;
                        float losRate_accel = (dPos.x * relV.y - dPos.y * relV.x) / (dist * dist);
                        float a_t_perp = -targetAccel.x * sinf(los) + targetAccel.y * cosf(los);
                        float N = MISSIL_GUIANCA_APN_GANHO;
                        aCmd = N * Vc * losRate_accel + (N * 0.5f) * a_t_perp;
                    }
                    else if (p->guidance == GUIDANCE_PP)
                    {
                        float err = los - gamma;
                        while (err > (float)M_PI)
                            err -= 2.0f * (float)M_PI;
                        while (err < -(float)M_PI)
                            err += 2.0f * (float)M_PI;
                        aCmd = MISSIL_GUIANCA_PP_GANHO * speed * err;
                    }

                    if (aCmd > p->maxLatAccel)
                        aCmd = p->maxLatAccel;
                    if (aCmd < -p->maxLatAccel)
                        aCmd = -p->maxLatAccel;

                    p->actualLatAccel += (aCmd - p->actualLatAccel) * (delta_tempo / MISSIL_GUIANCA_LAG);
                }
            }

            if (!p->missed)
            {
                float a_x_lateral = -p->actualLatAccel * sinf(gamma);
                float a_y_lateral = p->actualLatAccel * cosf(gamma);
                p->vel.x += a_x_lateral * delta_tempo;
                p->vel.y += a_y_lateral * delta_tempo;

                float newSpeed = matematica_vetor2d_len(p->vel);
                if (newSpeed > 0.001f)
                    p->vel = matematica_vetor2d_multiplicacao(p->vel, speed / newSpeed);
            }
        }

        p->life -= delta_tempo;
        p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, delta_tempo));

        if (p->life <= 0.0f || p->pos.x < -20.0f || p->pos.x > jogo->width + 20.0f ||
            p->pos.y < -20.0f || p->pos.y > jogo->height + 20.0f)
        {
            p->active = 0;
        }

        for (oi = 0; oi < MAXIMO_PLATAFORMAS && p->active; ++oi)
        {
            Obstacle *o = &jogo->obstacles[oi];
            if (!o->active)
                continue;
            if (colisao_circulo_vs_retangulo(p->pos, p->radius,
                                             matematica_vetor2d(o->x, o->y),
                                             matematica_vetor2d(o->x + o->w, o->y + o->h)))
                p->active = 0;
        }
    }
}

static int gameplay_atualizar_inimigos_internos(Game *jogo, float delta_tempo, float scoreMul)
{
    int i;
    int tookHit = 0;

    for (i = 0; i < MAXIMO_INIMIGOS; ++i)
    {
        Enemy *e = &jogo->enemies[i];
        int j;
        Vetor2D ep;
        if (!e->active)
            continue;

        e->angle += e->angularSpeed * delta_tempo;
        e->hitFlash -= delta_tempo * 4.0f;
        if (e->hitFlash < 0.0f)
            e->hitFlash = 0.0f;

        ep = inimigo_posicao(e);

        {
            static Vetor2D prevEnemyPos[MAXIMO_INIMIGOS] = {{0, 0}};
            static Vetor2D prevEnemyVel[MAXIMO_INIMIGOS] = {{0, 0}};
            if (prevEnemyPos[i].x == 0 && prevEnemyPos[i].y == 0)
                prevEnemyPos[i] = ep;
            e->vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(ep, prevEnemyPos[i]), 1.0f / delta_tempo);
            e->accel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(e->vel, prevEnemyVel[i]), 1.0f / delta_tempo);
            prevEnemyPos[i] = ep;
            prevEnemyVel[i] = e->vel;
        }

        if (colisao_circulo_vs_circulo(jogo->player.pos, jogo->player.size * 0.8f, ep, e->size))
        {
            jogo->player.hp = matematica_limite_min(jogo->player.hp -= e->damage * delta_tempo, 0);
            tookHit = 1;
            particulas_criar(jogo, jogo->player.pos, 1, (Color){1.0f, 0.2f, 0.2f, 0.8f});
        }

        e->shootCooldown -= delta_tempo;
        if (e->shootCooldown <= 0.0f)
        {
            Vetor2D dirToPlayer = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(jogo->player.pos, ep));
            float enemyShotSpeed;
            float enemyShotDamage;

            if (e->type == ENEMY_DIAMOND)
            {
                if (e->burstCount > 0)
                {
                    float ppOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 0.5f;
                    projeteis_criar(jogo, ep, dirToPlayer, 0, 200.0f + jogo->wave * 10.0f, 25.0f + jogo->wave * 4.0f, 6.0f, 4.0f, GUIDANCE_PP, -1, ppOverload);
                    e->burstCount--;
                    e->shootCooldown = 0.5f;
                    if (e->burstCount == 0)
                    {
                        e->shootCooldown = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                        e->burstCount = 2;
                    }
                }
                else
                {
                    e->shootCooldown = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                    e->burstCount = 2;
                }
            }
            else if (e->type == ENEMY_PENTAGON)
            {
                float apnOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 1.0f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, 300.0f + jogo->wave * 15.0f, 30.0f + jogo->wave * 5.0f, 6.0f, 6.0f, GUIDANCE_APNG, -1, apnOverload);
                e->shootCooldown = matematica_float_aleatorio_alcance(2.5f, 4.0f);
            }
            else if (e->isBoss)
            {
                Vetor2D sideA = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f - dirToPlayer.y * 0.38f, dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                Vetor2D sideB = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f + dirToPlayer.y * 0.38f, -dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                enemyShotSpeed = 260.0f + jogo->wave * 18.0f;
                enemyShotDamage = 9.5f + jogo->wave * 1.4f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 8.0f, 4.4f, GUIDANCE_NONE, -1, 0.0f);
                projeteis_criar(jogo, ep, sideA, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                projeteis_criar(jogo, ep, sideB, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio_alcance(0.8f, 1.8f) - jogo->wave * 0.05f;
            }
            else if (e->type == ENEMY_SNIPER)
            {
                enemyShotSpeed = 330.0f + jogo->wave * 22.0f;
                enemyShotDamage = 9.0f + jogo->wave * 1.6f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 5.5f, 3.6f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio_alcance(1.0f, 2.1f) - jogo->wave * 0.04f;
            }
            else if (e->type == ENEMY_TANK)
            {
                enemyShotSpeed = 180.0f + jogo->wave * 14.0f;
                enemyShotDamage = 11.0f + jogo->wave * 1.8f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 9.0f, 4.8f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio_alcance(1.8f, 3.2f) - jogo->wave * 0.03f;
            }
            else
            {
                enemyShotSpeed = 220.0f + jogo->wave * 18.0f;
                enemyShotDamage = 7.0f + jogo->wave * 1.4f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio_alcance(1.1f, 2.6f) - jogo->wave * 0.05f;
            }

            if (e->shootCooldown < 0.45f)
                e->shootCooldown = 0.45f;
        }

        for (j = 0; j < MAXIMO_PROJETEIS; ++j)
        {
            Projectile *p = &jogo->projectiles[j];
            if (!p->active || !p->fromPlayer)
                continue;

            if (colisao_circulo_vs_circulo(p->pos, p->radius, ep, e->size))
            {
                p->active = 0;
                e->hp = matematica_limite_min(e->hp -= p->damage, 0);
                e->hitFlash = 1.0f;
                particulas_criar(jogo, ep, 8, (Color){1.0f, 0.6f, 0.2f, 0.95f});
                audio_tocar_som_tiro_atingido();

                if (e->hp <= 0.0f)
                {
                    e->active = 0;
                    jogo->enemiesRemaining--;
                    if (e->isBoss)
                    {
                        jogo->score += (int)(1500.0f * scoreMul);
                        jogo->gold += 6;
                    }
                    else if (e->type == ENEMY_TANK)
                    {
                        jogo->score += (int)((230 + jogo->wave * 28) * scoreMul);
                        jogo->gold += 2;
                    }
                    else if (e->type == ENEMY_SNIPER)
                    {
                        jogo->score += (int)((150 + jogo->wave * 24) * scoreMul);
                        jogo->gold += 1;
                    }
                    else
                    {
                        jogo->score += (int)((120 + jogo->wave * 20) * scoreMul);
                        jogo->gold += 1;
                    }
                    particulas_criar(jogo, ep, 18, (Color){1.0f, 0.85f, 0.2f, 0.95f});
                }
                break;
            }
        }
    }
    return tookHit;
}

static int gameplay_verificar_projeteis_vs_jogador(Game *jogo)
{
    int i;
    int tookHit = 0;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &jogo->projectiles[i];
        if (!p->active || p->fromPlayer)
            continue;

        if (colisao_circulo_vs_circulo(p->pos, p->radius, jogo->player.pos, jogo->player.size * 0.75f))
        {
            p->active = 0;
            jogo->player.hp = matematica_limite_min(jogo->player.hp -= p->damage, 0);
            tookHit = 1;
            particulas_criar(jogo, jogo->player.pos, 7, (Color){1.0f, 0.3f, 0.2f, 0.9f});
            audio_tocar_som_tiro_atingido();
        }
    }
    return tookHit;
}

static void gameplay_atualizar_particulas_internos(Game *jogo, float delta_tempo)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &jogo->particles[i];
        if (!pt->active)
            continue;
        pt->life -= delta_tempo;
        pt->pos = matematica_vetor2d_adicao(pt->pos, matematica_vetor2d_multiplicacao(pt->vel, delta_tempo));
        pt->vel = matematica_vetor2d_multiplicacao(pt->vel, 0.96f);
        if (pt->life <= 0.0f)
            pt->active = 0;
    }
}

void gameplay_atualizar_playing(Game *jogo, float delta_tempo)
{
    int tookHit;
    float scoreMul = inimigo_multiplicador_pontos(jogo->difficulty);

    gameplay_atualizar_jogador_movimento(jogo, delta_tempo);
    gameplay_atirar_jogador(jogo, delta_tempo);
    gameplay_atualizar_projeteis_internos(jogo, delta_tempo);
    tookHit = gameplay_atualizar_inimigos_internos(jogo, delta_tempo, scoreMul);
    tookHit |= gameplay_verificar_projeteis_vs_jogador(jogo);
    gameplay_atualizar_particulas_internos(jogo, delta_tempo);

    jogo->elapsed += delta_tempo;
    jogo->timeLeft -= delta_tempo;
    jogo->score += (int)(delta_tempo * 14.0f * scoreMul);
    jogo->damageFlash -= delta_tempo * 2.4f;
    if (jogo->damageFlash < 0.0f)
        jogo->damageFlash = 0.0f;
    if (tookHit)
        jogo->damageFlash = 0.9f;

    if (jogo->player.hp <= 0.0f || jogo->timeLeft <= 0.0f)
    {
        interface_encerrar_partida(jogo, SCREEN_LOSE);
        return;
    }

    if (jogo->enemiesRemaining <= 0)
    {
        if (jogo->wave >= jogo->wavesToWin)
        {
            interface_encerrar_partida(jogo, SCREEN_WIN);
        }
        else
        {
            jogo->timeLeft += 6.0f;
            jogo->score += (int)((180 + jogo->wave * 40) * scoreMul);
            melhorias_rolar_opcoes(jogo);
            jogo->screen = SCREEN_UPGRADE;
        }
    }
}
