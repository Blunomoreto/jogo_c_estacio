#include "jogo.h"
#include "audio.h"
#include "colisao.h"
#include "configuracao.h"
#include "inimigo.h"
#include "imagem.h"
#include "matematica.h"
#include "particulas.h"
#include "persistencia.h"
#include "projeteis.h"
#include "renderizar.h"
#include "desenhar.h"
#include "interface.h"
#include "melhorias.h"
#include "cenario.h"

#include <GL/glut.h>
#include <ctype.h>
#include <math.h>
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

static void jogo_atualizar_playing(Game *jogo, float delta_tempo)
{
    int i;
    int tookHit = 0;
    float scoreMul = inimigo_multiplicador_pontos(jogo->difficulty);

    float moveX = 0.0f;
    if (jogo->input.keys['a'] || jogo->input.keys['A'] || jogo->input.special[GLUT_KEY_LEFT])
    {
        moveX -= 1.0f;
    }
    if (jogo->input.keys['d'] || jogo->input.keys['D'] || jogo->input.special[GLUT_KEY_RIGHT])
    {
        moveX += 1.0f;
    }
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
        {
            audio_tocar_som_pulo_fim();
        }
        jogo->player.pos.y = ALTURA_CHAO - jogo->player.size;
        jogo->player.velY = 0.0f;
        jogo->player.isOnGround = 1;
    }

    if (jogo->player.pos.y - jogo->player.size <= 20.0f)
    {
        jogo->player.pos.y = 20.0f + jogo->player.size;
        jogo->player.velY = 0.0f;
    }

    {
        int oi;
        for (oi = 0; oi < MAXIMO_PLATAFORMAS; ++oi)
        {
            Obstacle *o = &jogo->obstacles[oi];
            if (!o->active)
            {
                continue;
            }
            if (colisao_circulo_vs_retangulo(jogo->player.pos, jogo->player.size,
                                             matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->w, o->y + o->h)))
            {
                float cx = o->x + o->w * 0.5f;
                float cy = o->y + o->h * 0.5f;
                float dx = fabsf(jogo->player.pos.x - cx);
                float dy = fabsf(jogo->player.pos.y - cy);

                if (jogo->player.pos.y - jogo->player.size < cy && jogo->player.velY >= 0.0f)
                {
                    if (jogo->player.velY > 50.0f)
                    {
                        audio_tocar_som_pulo_fim();
                    }
                    jogo->player.pos.y = o->y - jogo->player.size;
                    jogo->player.velY = 0.0f;
                    jogo->player.isOnGround = 1;
                }
                else if (jogo->player.pos.y - jogo->player.size >= cy && dx > dy)
                {
                    if (jogo->player.pos.x < cx)
                    {
                        jogo->player.pos.x = o->x - jogo->player.size - 5.0f;
                    }
                    else
                    {
                        jogo->player.pos.x = o->x + o->w + jogo->player.size + 5.0f;
                    }
                    if (jogo->player.pos.x < jogo->player.size)
                        jogo->player.pos.x = jogo->player.size;
                    if (jogo->player.pos.x > jogo->width - jogo->player.size)
                        jogo->player.pos.x = jogo->width - jogo->player.size;
                }
            }
        }
    }

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
            {
                p->active = 0;
            }
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
                {
                    p->vel = matematica_vetor2d_multiplicacao(p->vel, speed / newSpeed);
                }
            }
        }

        p->life -= delta_tempo;
        p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, delta_tempo));

        if (p->life <= 0.0f || p->pos.x < -20.0f || p->pos.x > jogo->width + 20.0f || p->pos.y < -20.0f || p->pos.y > jogo->height + 20.0f)
        {
            p->active = 0;
        }

        for (oi = 0; oi < MAXIMO_PLATAFORMAS && p->active; ++oi)
        {
            Obstacle *o = &jogo->obstacles[oi];
            if (!o->active)
                continue;
            if (colisao_circulo_vs_retangulo(p->pos, p->radius, matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->w, o->y + o->h)))
                p->active = 0;
        }
    }

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
            {
                prevEnemyPos[i] = ep;
            }

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
            {
                e->shootCooldown = 0.45f;
            }
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

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &jogo->projectiles[i];
        if (!p->active || p->fromPlayer)
        {
            continue;
        }

        if (colisao_circulo_vs_circulo(p->pos, p->radius, jogo->player.pos, jogo->player.size * 0.75f))
        {
            p->active = 0;
            jogo->player.hp = matematica_limite_min(jogo->player.hp -= p->damage, 0);
            tookHit = 1;
            particulas_criar(jogo, jogo->player.pos, 7, (Color){1.0f, 0.3f, 0.2f, 0.9f});
            audio_tocar_som_tiro_atingido();
        }
    }

    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &jogo->particles[i];
        if (!pt->active)
            continue;

        pt->life -= delta_tempo;
        pt->pos = matematica_vetor2d_adicao(pt->pos, matematica_vetor2d_multiplicacao(pt->vel, delta_tempo));
        pt->vel = matematica_vetor2d_multiplicacao(pt->vel, 0.96f);
        if (pt->life <= 0.0f)
        {
            pt->active = 0;
        }
    }

    jogo->elapsed += delta_tempo;
    jogo->timeLeft -= delta_tempo;
    jogo->score += (int)(delta_tempo * 14.0f * scoreMul);
    jogo->damageFlash -= delta_tempo * 2.4f;
    if (jogo->damageFlash < 0.0f)
    {
        jogo->damageFlash = 0.0f;
    }
    if (tookHit)
    {
        jogo->damageFlash = 0.9f;
    }

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
    {
        return;
    }

    jogo->deltaTime = delta_tempo;

    jogo->upgradeFlash -= delta_tempo * 1.7f;
    if (jogo->upgradeFlash < 0.0f)
    {
        jogo->upgradeFlash = 0.0f;
    }
    jogo->lastUpgradeTimer -= delta_tempo;
    if (jogo->lastUpgradeTimer < 0.0f)
    {
        jogo->lastUpgradeTimer = 0.0f;
    }
    jogo->toastTimer -= delta_tempo;
    if (jogo->toastTimer < 0.0f)
    {
        jogo->toastTimer = 0.0f;
    }

    if (jogo->screen == SCREEN_PLAYING)
    {
        jogo_atualizar_playing(jogo, delta_tempo);
    }
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

    if (jogo->screen == SCREEN_PLAYING || jogo->screen == SCREEN_PAUSED || jogo->screen == SCREEN_UPGRADE || jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE)
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

void jogo_tecla_pressionada(Game *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla < 256)
    {
        jogo->input.keys[tecla] = 1;
        jogo->input.keysPressed[tecla] = 1;
    }

    if (tecla == 27)
    {
        if (jogo->screen == SCREEN_PLAYING)
        {
            jogo->screen = SCREEN_PAUSED;
        }
        else if (jogo->screen == SCREEN_PAUSED)
        {
            jogo->screen = SCREEN_PLAYING;
        }
        else if (jogo->screen == SCREEN_OPTIONS)
        {
            jogo->screen = SCREEN_MENU;
        }
        else if (jogo->screen == SCREEN_SCORES)
        {
            jogo->screen = SCREEN_MENU;
        }
    }

    if ((tecla == 'p' || tecla == 'P') && jogo->screen == SCREEN_PLAYING)
    {
        jogo->screen = SCREEN_PAUSED;
    }
    else if ((tecla == 'p' || tecla == 'P') && jogo->screen == SCREEN_PAUSED)
    {
        jogo->screen = SCREEN_PLAYING;
    }

    if (jogo->screen == SCREEN_MENU && (tecla == 13 || tecla == ' '))
    {
        jogo_reiniciar(jogo);
    }

    if (jogo->screen == SCREEN_MENU && (tecla == 'o' || tecla == 'O'))
    {
        jogo->screen = SCREEN_OPTIONS;
    }

    if (jogo->screen == SCREEN_MENU && (tecla == 'l' || tecla == 'L'))
    {
        jogo->screen = SCREEN_SCORES;
    }

    if (jogo->screen == SCREEN_PAUSED && tecla == 13)
    {
        jogo->screen = SCREEN_PLAYING;
    }

    if (jogo->screen == SCREEN_UPGRADE)
    {
        if (tecla == '1' || tecla == '2' || tecla == '3')
        {
            int idx = tecla - '1';
            if (idx >= 0 && idx < MAXIMO_OPCOES_UPGRADE)
            {
                melhorias_escolher(jogo, idx);
            }
        }
        else if (tecla == 'r' || tecla == 'R')
        {
            if (jogo->gold >= 3)
            {
                jogo->gold -= 3;
                melhorias_rolar_opcoes(jogo);
                interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            }
            else
            {
                interface_notificar(jogo, "Not enough gold for reroll");
            }
        }
    }

    if (jogo->screen == SCREEN_OPTIONS)
    {
        if (tecla == 'a' || tecla == 'A')
        {
            jogo->audioEnabled = !jogo->audioEnabled;
            audio_definir_ativacao(jogo->audioEnabled);
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            interface_notificar(jogo, jogo->audioEnabled ? "Audio ON" : "Audio OFF");
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->difficulty = (jogo->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->difficulty));
                interface_notificar(jogo, msg);
            }
        }
        else if (tecla == 13 || tecla == 'm' || tecla == 'M')
        {
            jogo->screen = SCREEN_MENU;
        }
    }

    if (jogo->screen == SCREEN_SCORES)
    {
        int pageCount = (jogo->allScoreCount + jogo->scorePageSize - 1) / jogo->scorePageSize;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        if (tecla == 'a' || tecla == 'A')
        {
            jogo->scorePage--;
            if (jogo->scorePage < 0)
            {
                jogo->scorePage = 0;
            }
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->scorePage++;
            if (jogo->scorePage > pageCount - 1)
            {
                jogo->scorePage = pageCount - 1;
            }
        }
        else if (tecla == 'c' || tecla == 'C')
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->scorePage = 0;
            interface_notificar(jogo, "Score history cleared");
        }
        else if (tecla == 'm' || tecla == 'M' || tecla == 13)
        {
            jogo->screen = SCREEN_MENU;
        }
    }

    if ((jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE) && jogo->enteringName)
    {
        int len = (int)strlen(jogo->playerName);

        if (tecla == 8 && len > 0)
        {
            jogo->playerName[len - 1] = '\0';
            return;
        }

        if (tecla == 13 && !jogo->nameSaved)
        {
            if (strlen(jogo->playerName) == 0)
            {
                snprintf(jogo->playerName, sizeof(jogo->playerName), "Player");
            }
            persistencia_apor_pontuacao(jogo->playerName, jogo->score, jogo->wave);
            jogo->nameSaved = 1;
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            interface_notificar(jogo, "Score saved");
            return;
        }

        if ((isalnum(tecla) || tecla == ' ' || tecla == '_') && len < (int)sizeof(jogo->playerName) - 1)
        {
            jogo->playerName[len] = (char)tecla;
            jogo->playerName[len + 1] = '\0';
        }

        if (jogo->nameSaved && (tecla == 'm' || tecla == 'M' || tecla == 13))
        {
            jogo->screen = SCREEN_MENU;
            jogo->enteringName = 0;
        }
    }
}

void jogo_tecla_levantada(Game *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
    {
        jogo->input.keys[tecla] = 0;
    }
}

void jogo_especial_pressionado(Game *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
    {
        jogo->input.special[tecla] = 1;
    }

    if (tecla == GLUT_KEY_F12)
    {
        time_t t = time(NULL);
        struct tm *tmv = localtime(&t);
        char filename[128];

        snprintf(filename, sizeof(filename), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.ppm",
                 tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
        if (imagem_salvar_ppm(filename, jogo->width, jogo->height))
        {
            interface_notificar(jogo, "Screenshot saved");
        }
        else
        {
            interface_notificar(jogo, "Screenshot failed");
        }
    }
}

void jogo_especial_levantado(Game *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
    {
        jogo->input.special[tecla] = 0;
    }
}

void jogo_mouse_pressionado(Game *jogo, int botao, int estado, int x, int y)
{
    float wx;
    float wy;
    float bx;
    float by;

    jogo->input.mouseX = x;
    jogo->input.mouseY = y;

    if (botao >= 0 && botao < 3)
    {
        if (estado == GLUT_DOWN)
        {
            jogo->input.mouseDown[botao] = 1;
            jogo->input.mousePressed[botao] = 1;
        }
        else
        {
            jogo->input.mouseDown[botao] = 0;
        }
    }

    wx = (float)x;
    wy = (float)y;

    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN)
    {
        return;
    }

    if (jogo->screen == SCREEN_MENU)
    {
        float uiScale = (float)jogo->width / 1280.0f;
        float hScale = (float)jogo->height / 720.0f;
        float btnW;
        float startH;
        float subH;
        if (hScale < uiScale)
        {
            uiScale = hScale;
        }
        if (uiScale < 0.75f)
        {
            uiScale = 0.75f;
        }
        if (uiScale > 1.15f)
        {
            uiScale = 1.15f;
        }

        btnW = 300.0f * uiScale;
        startH = 60.0f * uiScale;
        subH = 50.0f * uiScale;
        bx = jogo->width * 0.5f - btnW * 0.5f;
        by = jogo->height * 0.30f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + startH)
        {
            jogo_reiniciar(jogo);
            return;
        }

        by = jogo->height * 0.40f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->screen = SCREEN_OPTIONS;
            return;
        }

        by = jogo->height * 0.50f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->screen = SCREEN_SCORES;
            return;
        }
    }

    if (jogo->screen == SCREEN_OPTIONS)
    {
        bx = jogo->width * 0.5f - 180.0f;
        by = jogo->height * 0.56f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->audioEnabled = !jogo->audioEnabled;
            audio_definir_ativacao(jogo->audioEnabled);
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            interface_notificar(jogo, jogo->audioEnabled ? "Audio ON" : "Audio OFF");
            return;
        }

        by = jogo->height * 0.46f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->difficulty = (jogo->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->difficulty));
                interface_notificar(jogo, msg);
            }
            return;
        }

        bx = jogo->width * 0.5f - 110.0f;
        by = jogo->height * 0.31f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->screen = SCREEN_MENU;
            return;
        }
    }

    if (jogo->screen == SCREEN_SCORES)
    {
        int pageCount = (jogo->allScoreCount + jogo->scorePageSize - 1) / jogo->scorePageSize;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        bx = jogo->width * 0.5f - 255.0f;
        by = jogo->height * 0.20f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->scorePage--;
            if (jogo->scorePage < 0)
            {
                jogo->scorePage = 0;
            }
            return;
        }

        bx = jogo->width * 0.5f + 95.0f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->scorePage++;
            if (jogo->scorePage > pageCount - 1)
            {
                jogo->scorePage = pageCount - 1;
            }
            return;
        }

        bx = jogo->width * 0.5f - 110.0f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->scorePage = 0;
            interface_notificar(jogo, "Score history cleared");
            return;
        }

        by = jogo->height * 0.11f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->screen = SCREEN_MENU;
            return;
        }
    }

    if (jogo->screen == SCREEN_UPGRADE)
    {
        float uiScale = (float)jogo->width / 1280.0f;
        float hScale = (float)jogo->height / 720.0f;
        float centerX = jogo->width * 0.5f;
        float cardW;
        float cardH;
        float cardGap;
        float totalW;
        float startX;
        float cardY;
        float rerollW;
        float rerollH;
        float rerollX;
        float rerollY;
        int i;

        if (hScale < uiScale)
        {
            uiScale = hScale;
        }
        if (uiScale < 0.78f)
        {
            uiScale = 0.78f;
        }
        if (uiScale > 1.08f)
        {
            uiScale = 1.08f;
        }

        cardW = 170.0f * uiScale;
        cardH = 130.0f * uiScale;
        cardGap = 20.0f * uiScale;
        totalW = cardW * MAXIMO_OPCOES_UPGRADE + cardGap * (MAXIMO_OPCOES_UPGRADE - 1);
        startX = centerX - totalW * 0.5f;
        cardY = jogo->height * 0.36f;

        rerollW = 220.0f * uiScale;
        rerollH = 44.0f * uiScale;
        rerollX = centerX - rerollW * 0.5f;
        rerollY = cardY + cardH + 20.0f * uiScale;

        for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
        {
            bx = startX + i * (cardW + cardGap);
            by = cardY;
            if (wx >= bx && wx <= bx + cardW && wy >= by && wy <= by + cardH)
            {
                melhorias_escolher(jogo, i);
                return;
            }
        }

        bx = rerollX;
        by = rerollY;
        if (wx >= bx && wx <= bx + rerollW && wy >= by && wy <= by + rerollH && jogo->gold >= 3)
        {
            jogo->gold -= 3;
            melhorias_rolar_opcoes(jogo);
            interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE) && jogo->nameSaved)
    {
        bx = jogo->width * 0.5f - 110.0f;
        by = jogo->height * 0.45f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->screen = SCREEN_MENU;
            jogo->enteringName = 0;
        }
    }
}

void jogo_mouse_movido(Game *jogo, int x, int y)
{
    jogo->input.mouseX = x;
    jogo->input.mouseY = y;
}

void jogo_iniciar_frame(Game *jogo)
{
    memset(jogo->input.keysPressed, 0, sizeof(jogo->input.keysPressed));
    memset(jogo->input.mousePressed, 0, sizeof(jogo->input.mousePressed));
}
