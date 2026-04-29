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
#include "print.h"
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

static void game_reset_player(Game *g)
{
    memset(&g->player, 0, sizeof(g->player));

    g->player.pos = matematica_vetor2d((float)g->width * 0.5f, ALTURA_CHAO - 16.0f);
    g->player.size = 16.0f;
    g->player.hp = JOGADOR_INICIAL_VIDA;
    g->player.maxHp = JOGADOR_INICIAL_VIDA;
    g->player.speed = JOGADOR_INICIAL_VELOCIDADE;
    g->player.damage = JOGADOR_INICIAL_DANO;
    g->player.fireRate = JOGADOR_INICIAL_TAXA_DISPARO;
    g->player.fireCooldown = 0.0f;
    g->player.projectileSpeed = JOGADOR_INICIAL_VELOCIDADE_PROJETIL;
    g->player.velY = 0.0f;
    g->player.isOnGround = 1;
    g->player.jumpPressedTime = 0.0f;

    g->player.hasPP = 0;
    g->player.hasAPNG = 0;
    g->player.guidedAmmo = 0;
    g->player.maxGuidedAmmo = 0;
    g->player.maxLatAccel = JOGADOR_INICIAL_ACELERACAO_LATERAL;
}

void jogo_reiniciar(Game *g)
{
    game_reset_player(g);
    cenario_limpar_entidades(g);

    g->wave = 1;
    g->wavesToWin = inimigo_ondas_vitoria(g->difficulty);
    g->enemiesRemaining = 0;
    g->timeLeft = inimigo_tempo_inicio(g->difficulty);
    g->elapsed = 0.0f;
    g->score = 0;
    g->gold = 0;
    g->damageFlash = 0.0f;
    g->upgradeFlash = 0.0f;
    g->upgradeHover = -1;
    g->lastUpgradeTimer = 0.0f;
    memset(g->lastUpgrade, 0, sizeof(g->lastUpgrade));
    g->toastTimer = 0.0f;
    memset(g->toastMessage, 0, sizeof(g->toastMessage));
    g->screen = SCREEN_PLAYING;
    g->enteringName = 0;
    g->nameSaved = 0;

    memset(g->playerName, 0, sizeof(g->playerName));
    snprintf(g->playerName, sizeof(g->playerName), "Player");

    cenario_criar_onda(g);
    audio_tocar_musica();
}

void jogo_iniciar(Game *g, int width, int height)
{
    memset(g, 0, sizeof(*g));
    srand((unsigned int)time(NULL));

    g->width = width;
    g->height = height;
    g->screen = SCREEN_MENU;
    g->running = 1;
    g->audioEnabled = 1;
    g->difficulty = 1;
    g->scorePage = 0;
    g->scorePageSize = 10;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    persistencia_carregar_stats(&g->highScore, &g->maxWaveEver);
    persistencia_carregar_configuracoes(&g->audioEnabled, &g->difficulty);
    interface_refrescar_pontuacoes_maximas(g);
    interface_refrescar_pontuacoes(g);

    g->bgTexture = imagem_carregar_ppm("assets/images/background.ppm", &g->bgTextureLoaded);
    audio_inicializar();
    audio_definir_ativacao(g->audioEnabled);
}

static void jogo_atualizar_playing(Game *g, float dt)
{
    int i;
    int tookHit = 0;
    float scoreMul = inimigo_multiplicador_pontos(g->difficulty);

    float moveX = 0.0f;
    if (g->input.keys['a'] || g->input.keys['A'] || g->input.special[GLUT_KEY_LEFT])
    {
        moveX -= 1.0f;
    }
    if (g->input.keys['d'] || g->input.keys['D'] || g->input.special[GLUT_KEY_RIGHT])
    {
        moveX += 1.0f;
    }
    g->player.pos.x += moveX * g->player.speed * dt;

    if (g->player.pos.x < g->player.size)
        g->player.pos.x = g->player.size;
    if (g->player.pos.x > g->width - g->player.size)
        g->player.pos.x = g->width - g->player.size;

    if (g->input.keys[' '] || g->input.keys['w'] || g->input.keys['W'])
    {
        if (g->player.isOnGround)
        {
            g->player.velY = -JOGADOR_FORCA_PULO;
            g->player.isOnGround = 0;
            audio_tocar_som_pulo_inicio();
        }
    }

    g->player.velY += GRAVIDADE * dt;

    if (g->player.velY > 600.0f)
        g->player.velY = 600.0f;

    g->player.pos.y += g->player.velY * dt;

    {
        static Vetor2D prevP = {0, 0};
        static Vetor2D prevV = {0, 0};
        if (prevP.x == 0 && prevP.y == 0)
            prevP = g->player.pos;
        g->player.vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(g->player.pos, prevP), 1.0f / dt);
        g->player.accel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(g->player.vel, prevV), 1.0f / dt);
        prevP = g->player.pos;
        prevV = g->player.vel;
    }

    g->player.isOnGround = 0;
    if (g->player.pos.y + g->player.size >= ALTURA_CHAO)
    {
        if (g->player.velY > 100.0f)
        {
            audio_tocar_som_pulo_fim();
        }
        g->player.pos.y = ALTURA_CHAO - g->player.size;
        g->player.velY = 0.0f;
        g->player.isOnGround = 1;
    }

    if (g->player.pos.y - g->player.size <= 20.0f)
    {
        g->player.pos.y = 20.0f + g->player.size;
        g->player.velY = 0.0f;
    }

    {
        int oi;
        for (oi = 0; oi < MAXIMO_PLATAFORMAS; ++oi)
        {
            Obstacle *o = &g->obstacles[oi];
            if (!o->active)
            {
                continue;
            }
            if (colisao_circulo_vs_retangulo(g->player.pos, g->player.size,
                                         matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->w, o->y + o->h)))
            {
                float cx = o->x + o->w * 0.5f;
                float cy = o->y + o->h * 0.5f;
                float dx = fabsf(g->player.pos.x - cx);
                float dy = fabsf(g->player.pos.y - cy);

                if (g->player.pos.y - g->player.size < cy && g->player.velY >= 0.0f)
                {
                    if (g->player.velY > 50.0f)
                    {
                        audio_tocar_som_pulo_fim();
                    }
                    g->player.pos.y = o->y - g->player.size;
                    g->player.velY = 0.0f;
                    g->player.isOnGround = 1;
                }
                else if (g->player.pos.y - g->player.size >= cy && dx > dy)
                {
                    if (g->player.pos.x < cx)
                    {
                        g->player.pos.x = o->x - g->player.size - 5.0f;
                    }
                    else
                    {
                        g->player.pos.x = o->x + o->w + g->player.size + 5.0f;
                    }
                    if (g->player.pos.x < g->player.size)
                        g->player.pos.x = g->player.size;
                    if (g->player.pos.x > g->width - g->player.size)
                        g->player.pos.x = g->width - g->player.size;
                }
            }
        }
    }

    g->player.fireCooldown -= dt;

    if (g->input.mouseDown[0] && g->player.fireCooldown <= 0.0f)
    {
        Vetor2D target = matematica_mouse_para_mundo(g);
        Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, g->player.pos));
        projeteis_criar(g, g->player.pos, dir, 1, g->player.projectileSpeed, g->player.damage, 6.0f, 2.5f, GUIDANCE_NONE, -1, 0.0f);
        particulas_criar(g, g->player.pos, 4, (Color){0.3f, 0.9f, 1.0f, 0.85f});
        g->player.fireCooldown = g->player.fireRate;
        audio_tocar_som_tiro_disparo();
    }

    if (g->input.mouseDown[2] && g->player.fireCooldown <= 0.0f && g->player.guidedAmmo > 0)
    {
        if (g->player.hasPP || g->player.hasAPNG)
        {
            Vetor2D target = matematica_mouse_para_mundo(g);
            int bestTarget = -1;
            float minD = 1000.0f;
            for (i = 0; i < MAXIMO_INIMIGOS; ++i)
            {
                if (g->enemies[i].active)
                {
                    float d = matematica_vetor2d_len(matematica_vetor2d_subtracao(inimigo_posicao(&g->enemies[i]), target));
                    if (d < minD)
                    {
                        minD = d;
                        bestTarget = i;
                    }
                }
            }
            if (bestTarget != -1)
            {
                Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, g->player.pos));
                GuidanceType law = g->player.hasAPNG ? GUIDANCE_APNG : GUIDANCE_PP;

                projeteis_criar(g, g->player.pos, dir, 1, g->player.speed * 1.3f, g->player.damage * 2.0f, 8.0f, 5.0f, law, bestTarget, g->player.maxLatAccel);
                g->player.guidedAmmo--;
                g->player.fireCooldown = g->player.fireRate * 2.0f;
                audio_tocar_som_tiro_disparo();
            }
        }
    }

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &g->projectiles[i];
        int oi;
        if (!p->active)
            continue;

        if (p->guidance != GUIDANCE_NONE && p->missed)
        {
            p->sdTimer -= dt;
            if (p->sdTimer <= 0.0f)
            {
                p->active = 0;
            }
            p->life -= dt;
            p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, dt));
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
                if (p->targetIdx != -1 && g->enemies[p->targetIdx].active)
                {
                    Enemy *e = &g->enemies[p->targetIdx];
                    targetPos = inimigo_posicao(e);
                    targetVel = e->vel;
                    targetAccel = e->accel;
                    targetValid = 1;
                }
            }
            else
            {
                targetPos = g->player.pos;
                targetVel = g->player.vel;
                targetAccel = g->player.accel;
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

                    p->actualLatAccel += (aCmd - p->actualLatAccel) * (dt / MISSIL_GUIANCA_LAG);
                }
            }

            if (!p->missed)
            {
                float a_x_lateral = -p->actualLatAccel * sinf(gamma);
                float a_y_lateral = p->actualLatAccel * cosf(gamma);

                p->vel.x += a_x_lateral * dt;
                p->vel.y += a_y_lateral * dt;

                float newSpeed = matematica_vetor2d_len(p->vel);
                if (newSpeed > 0.001f)
                {
                    p->vel = matematica_vetor2d_multiplicacao(p->vel, speed / newSpeed);
                }
            }
        }

        p->life -= dt;
        p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, dt));

        if (p->life <= 0.0f || p->pos.x < -20.0f || p->pos.x > g->width + 20.0f || p->pos.y < -20.0f || p->pos.y > g->height + 20.0f)
        {
            p->active = 0;
        }

        for (oi = 0; oi < MAXIMO_PLATAFORMAS && p->active; ++oi)
        {
            Obstacle *o = &g->obstacles[oi];
            if (!o->active)
                continue;
            if (colisao_circulo_vs_retangulo(p->pos, p->radius, matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->w, o->y + o->h)))
                p->active = 0;
        }
    }

    for (i = 0; i < MAXIMO_INIMIGOS; ++i)
    {
        Enemy *e = &g->enemies[i];
        int j;
        Vetor2D ep;
        if (!e->active)
            continue;

        e->angle += e->angularSpeed * dt;
        e->hitFlash -= dt * 4.0f;
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

            e->vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(ep, prevEnemyPos[i]), 1.0f / dt);
            e->accel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(e->vel, prevEnemyVel[i]), 1.0f / dt);

            prevEnemyPos[i] = ep;
            prevEnemyVel[i] = e->vel;
        }

        if (colisao_circulo_vs_circulo(g->player.pos, g->player.size * 0.8f, ep, e->size))
        {
            g->player.hp = matematica_limite_min(g->player.hp -= e->damage * dt, 0);
            tookHit = 1;
            particulas_criar(g, g->player.pos, 1, (Color){1.0f, 0.2f, 0.2f, 0.8f});
        }

        e->shootCooldown -= dt;
        if (e->shootCooldown <= 0.0f)
        {
            Vetor2D dirToPlayer = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(g->player.pos, ep));
            float enemyShotSpeed;
            float enemyShotDamage;

            if (e->type == ENEMY_DIAMOND)
            {
                if (e->burstCount > 0)
                {
                    float ppOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 0.5f;
                    projeteis_criar(g, ep, dirToPlayer, 0, 200.0f + g->wave * 10.0f, 25.0f + g->wave * 4.0f, 6.0f, 4.0f, GUIDANCE_PP, -1, ppOverload);
                    e->burstCount--;
                    e->shootCooldown = 0.5f;
                    if (e->burstCount == 0)
                    {
                        e->shootCooldown = matematica_float_aleatorio(3.0f, 7.0f);
                        e->burstCount = 3;
                    }
                }
                else
                {
                    e->shootCooldown = matematica_float_aleatorio(3.0f, 7.0f);
                    e->burstCount = 3;
                }
            }
            else if (e->type == ENEMY_PENTAGON)
            {
                float apnOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 1.0f;
                projeteis_criar(g, ep, dirToPlayer, 0, 300.0f + g->wave * 15.0f, 30.0f + g->wave * 5.0f, 6.0f, 6.0f, GUIDANCE_APNG, -1, apnOverload);
                e->shootCooldown = matematica_float_aleatorio(2.5f, 4.0f);
            }
            else if (e->isBoss)
            {
                Vetor2D sideA = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f - dirToPlayer.y * 0.38f, dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                Vetor2D sideB = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f + dirToPlayer.y * 0.38f, -dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                enemyShotSpeed = 260.0f + g->wave * 18.0f;
                enemyShotDamage = 9.5f + g->wave * 1.4f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 8.0f, 4.4f, GUIDANCE_NONE, -1, 0.0f);
                projeteis_criar(g, ep, sideA, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                projeteis_criar(g, ep, sideB, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio(0.8f, 1.8f) - g->wave * 0.05f;
            }
            else if (e->type == ENEMY_SNIPER)
            {
                enemyShotSpeed = 330.0f + g->wave * 22.0f;
                enemyShotDamage = 9.0f + g->wave * 1.6f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 5.5f, 3.6f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio(1.0f, 2.1f) - g->wave * 0.04f;
            }
            else if (e->type == ENEMY_TANK)
            {
                enemyShotSpeed = 180.0f + g->wave * 14.0f;
                enemyShotDamage = 11.0f + g->wave * 1.8f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 9.0f, 4.8f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio(1.8f, 3.2f) - g->wave * 0.03f;
            }
            else
            {
                enemyShotSpeed = 220.0f + g->wave * 18.0f;
                enemyShotDamage = 7.0f + g->wave * 1.4f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 7.0f, 4.0f, GUIDANCE_NONE, -1, 0.0f);
                e->shootCooldown = matematica_float_aleatorio(1.1f, 2.6f) - g->wave * 0.05f;
            }

            if (e->shootCooldown < 0.45f)
            {
                e->shootCooldown = 0.45f;
            }
        }

        for (j = 0; j < MAXIMO_PROJETEIS; ++j)
        {
            Projectile *p = &g->projectiles[j];
            if (!p->active || !p->fromPlayer)
                continue;

            if (colisao_circulo_vs_circulo(p->pos, p->radius, ep, e->size))
            {
                p->active = 0;
                e->hp = matematica_limite_min(e->hp -= p->damage, 0);
                e->hitFlash = 1.0f;
                particulas_criar(g, ep, 8, (Color){1.0f, 0.6f, 0.2f, 0.95f});
                audio_tocar_som_tiro_atingido();

                if (e->hp <= 0.0f)
                {
                    e->active = 0;
                    g->enemiesRemaining--;
                    if (e->isBoss)
                    {
                        g->score += (int)(1500.0f * scoreMul);
                        g->gold += 6;
                    }
                    else if (e->type == ENEMY_TANK)
                    {
                        g->score += (int)((230 + g->wave * 28) * scoreMul);
                        g->gold += 2;
                    }
                    else if (e->type == ENEMY_SNIPER)
                    {
                        g->score += (int)((150 + g->wave * 24) * scoreMul);
                        g->gold += 1;
                    }
                    else
                    {
                        g->score += (int)((120 + g->wave * 20) * scoreMul);
                        g->gold += 1;
                    }
                    particulas_criar(g, ep, 18, (Color){1.0f, 0.85f, 0.2f, 0.95f});
                }
                break;
            }
        }
    }

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &g->projectiles[i];
        if (!p->active || p->fromPlayer)
        {
            continue;
        }

        if (colisao_circulo_vs_circulo(p->pos, p->radius, g->player.pos, g->player.size * 0.75f))
        {
            p->active = 0;
            g->player.hp = matematica_limite_min(g->player.hp -= p->damage, 0);
            tookHit = 1;
            particulas_criar(g, g->player.pos, 7, (Color){1.0f, 0.3f, 0.2f, 0.9f});
            audio_tocar_som_tiro_atingido();
        }
    }

    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &g->particles[i];
        if (!pt->active)
            continue;

        pt->life -= dt;
        pt->pos = matematica_vetor2d_adicao(pt->pos, matematica_vetor2d_multiplicacao(pt->vel, dt));
        pt->vel = matematica_vetor2d_multiplicacao(pt->vel, 0.96f);
        if (pt->life <= 0.0f)
        {
            pt->active = 0;
        }
    }

    g->elapsed += dt;
    g->timeLeft -= dt;
    g->score += (int)(dt * 14.0f * scoreMul);
    g->damageFlash -= dt * 2.4f;
    if (g->damageFlash < 0.0f)
    {
        g->damageFlash = 0.0f;
    }
    if (tookHit)
    {
        g->damageFlash = 0.9f;
    }

    if (g->player.hp <= 0.0f || g->timeLeft <= 0.0f)
    {
        interface_desenhar_tela_fim(g, SCREEN_LOSE);
        return;
    }

    if (g->enemiesRemaining <= 0)
    {
        if (g->wave >= g->wavesToWin)
        {
            interface_desenhar_tela_fim(g, SCREEN_WIN);
        }
        else
        {
            g->timeLeft += 6.0f;
            g->score += (int)((180 + g->wave * 40) * scoreMul);
            melhorias_rolar_opcoes(g);
            g->screen = SCREEN_UPGRADE;
        }
    }
}

void jogo_atualizar(Game *g, float dt)
{
    if (!g->running)
    {
        return;
    }

    g->deltaTime = dt;

    g->upgradeFlash -= dt * 1.7f;
    if (g->upgradeFlash < 0.0f)
    {
        g->upgradeFlash = 0.0f;
    }
    g->lastUpgradeTimer -= dt;
    if (g->lastUpgradeTimer < 0.0f)
    {
        g->lastUpgradeTimer = 0.0f;
    }
    g->toastTimer -= dt;
    if (g->toastTimer < 0.0f)
    {
        g->toastTimer = 0.0f;
    }

    if (g->screen == SCREEN_PLAYING)
    {
        jogo_atualizar_playing(g, dt);
    }
}

void jogo_tecla_pressionada(Game *g, unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (key < 256)
    {
        g->input.keys[key] = 1;
        g->input.keysPressed[key] = 1;
    }

    if (key == 27)
    {
        if (g->screen == SCREEN_PLAYING)
        {
            g->screen = SCREEN_PAUSED;
        }
        else if (g->screen == SCREEN_PAUSED)
        {
            g->screen = SCREEN_PLAYING;
        }
        else if (g->screen == SCREEN_OPTIONS)
        {
            g->screen = SCREEN_MENU;
        }
        else if (g->screen == SCREEN_SCORES)
        {
            g->screen = SCREEN_MENU;
        }
    }

    if ((key == 'p' || key == 'P') && g->screen == SCREEN_PLAYING)
    {
        g->screen = SCREEN_PAUSED;
    }
    else if ((key == 'p' || key == 'P') && g->screen == SCREEN_PAUSED)
    {
        g->screen = SCREEN_PLAYING;
    }

    if (g->screen == SCREEN_MENU && (key == 13 || key == ' '))
    {
        jogo_reiniciar(g);
    }

    if (g->screen == SCREEN_MENU && (key == 'o' || key == 'O'))
    {
        g->screen = SCREEN_OPTIONS;
    }

    if (g->screen == SCREEN_MENU && (key == 'l' || key == 'L'))
    {
        g->screen = SCREEN_SCORES;
    }

    if (g->screen == SCREEN_PAUSED && key == 13)
    {
        g->screen = SCREEN_PLAYING;
    }

    if (g->screen == SCREEN_UPGRADE)
    {
        if (key == '1' || key == '2' || key == '3')
        {
            int idx = key - '1';
            if (idx >= 0 && idx < MAXIMO_OPCOES_UPGRADE)
            {
                melhorias_escolher(g, idx);
            }
        }
        else if (key == 'r' || key == 'R')
        {
            if (g->gold >= 3)
            {
                g->gold -= 3;
                melhorias_rolar_opcoes(g);
                interface_desenhar_notificacao(g, "Upgrade rerolled (-3 gold)");
            }
            else
            {
                interface_desenhar_notificacao(g, "Not enough gold for reroll");
            }
        }
    }

    if (g->screen == SCREEN_OPTIONS)
    {
        if (key == 'a' || key == 'A')
        {
            g->audioEnabled = !g->audioEnabled;
            audio_definir_ativacao(g->audioEnabled);
            persistencia_salvar_configuracoes(g->audioEnabled, g->difficulty);
            interface_desenhar_notificacao(g, g->audioEnabled ? "Audio ON" : "Audio OFF");
        }
        else if (key == 'd' || key == 'D')
        {
            g->difficulty = (g->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(g->audioEnabled, g->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(g->difficulty));
                interface_desenhar_notificacao(g, msg);
            }
        }
        else if (key == 13 || key == 'm' || key == 'M')
        {
            g->screen = SCREEN_MENU;
        }
    }

    if (g->screen == SCREEN_SCORES)
    {
        int pageCount = (g->allScoreCount + g->scorePageSize - 1) / g->scorePageSize;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        if (key == 'a' || key == 'A')
        {
            g->scorePage--;
            if (g->scorePage < 0)
            {
                g->scorePage = 0;
            }
        }
        else if (key == 'd' || key == 'D')
        {
            g->scorePage++;
            if (g->scorePage > pageCount - 1)
            {
                g->scorePage = pageCount - 1;
            }
        }
        else if (key == 'c' || key == 'C')
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            g->scorePage = 0;
            interface_desenhar_notificacao(g, "Score history cleared");
        }
        else if (key == 'm' || key == 'M' || key == 13)
        {
            g->screen = SCREEN_MENU;
        }
    }

    if ((g->screen == SCREEN_WIN || g->screen == SCREEN_LOSE) && g->enteringName)
    {
        int len = (int)strlen(g->playerName);

        if (key == 8 && len > 0)
        {
            g->playerName[len - 1] = '\0';
            return;
        }

        if (key == 13 && !g->nameSaved)
        {
            if (strlen(g->playerName) == 0)
            {
                snprintf(g->playerName, sizeof(g->playerName), "Player");
            }
            persistencia_apor_pontuacao(g->playerName, g->score, g->wave);
            g->nameSaved = 1;
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            interface_desenhar_notificacao(g, "Score saved");
            return;
        }

        if ((isalnum(key) || key == ' ' || key == '_') && len < (int)sizeof(g->playerName) - 1)
        {
            g->playerName[len] = (char)key;
            g->playerName[len + 1] = '\0';
        }

        if (g->nameSaved && (key == 'm' || key == 'M' || key == 13))
        {
            g->screen = SCREEN_MENU;
            g->enteringName = 0;
        }
    }
}

void jogo_tecla_levantada(Game *g, unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->input.keys[key] = 0;
    }
}

void jogo_especial_pressionado(Game *g, int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->input.special[key] = 1;
    }

    if (key == GLUT_KEY_F12)
    {
        time_t t = time(NULL);
        struct tm *tmv = localtime(&t);
        char filename[128];

        snprintf(filename, sizeof(filename), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.ppm",
                 tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
        if (print_capturar_ppm(filename, g->width, g->height))
        {
            interface_desenhar_notificacao(g, "Screenshot saved");
        }
        else
        {
            interface_desenhar_notificacao(g, "Screenshot failed");
        }
    }
}

void jogo_especial_levantado(Game *g, int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->input.special[key] = 0;
    }
}

void jogo_mouse_pressionado(Game *g, int button, int state, int x, int y)
{
    float wx;
    float wy;
    float bx;
    float by;

    g->input.mouseX = x;
    g->input.mouseY = y;

    if (button >= 0 && button < 3)
    {
        if (state == GLUT_DOWN)
        {
            g->input.mouseDown[button] = 1;
            g->input.mousePressed[button] = 1;
        }
        else
        {
            g->input.mouseDown[button] = 0;
        }
    }

    wx = (float)x;
    wy = (float)y;

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    {
        return;
    }

    if (g->screen == SCREEN_MENU)
    {
        float uiScale = (float)g->width / 1280.0f;
        float hScale = (float)g->height / 720.0f;
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
        bx = g->width * 0.5f - btnW * 0.5f;
        by = g->height * 0.30f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + startH)
        {
            jogo_reiniciar(g);
            return;
        }

        by = g->height * 0.40f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            g->screen = SCREEN_OPTIONS;
            return;
        }

        by = g->height * 0.50f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            g->screen = SCREEN_SCORES;
            return;
        }
    }

    if (g->screen == SCREEN_OPTIONS)
    {
        bx = g->width * 0.5f - 180.0f;
        by = g->height * 0.56f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            g->audioEnabled = !g->audioEnabled;
            audio_definir_ativacao(g->audioEnabled);
            persistencia_salvar_configuracoes(g->audioEnabled, g->difficulty);
            interface_desenhar_notificacao(g, g->audioEnabled ? "Audio ON" : "Audio OFF");
            return;
        }

        by = g->height * 0.46f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            g->difficulty = (g->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(g->audioEnabled, g->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(g->difficulty));
                interface_desenhar_notificacao(g, msg);
            }
            return;
        }

        bx = g->width * 0.5f - 110.0f;
        by = g->height * 0.31f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            g->screen = SCREEN_MENU;
            return;
        }
    }

    if (g->screen == SCREEN_SCORES)
    {
        int pageCount = (g->allScoreCount + g->scorePageSize - 1) / g->scorePageSize;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        bx = g->width * 0.5f - 255.0f;
        by = g->height * 0.20f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            g->scorePage--;
            if (g->scorePage < 0)
            {
                g->scorePage = 0;
            }
            return;
        }

        bx = g->width * 0.5f + 95.0f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            g->scorePage++;
            if (g->scorePage > pageCount - 1)
            {
                g->scorePage = pageCount - 1;
            }
            return;
        }

        bx = g->width * 0.5f - 110.0f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            g->scorePage = 0;
            interface_desenhar_notificacao(g, "Score history cleared");
            return;
        }

        by = g->height * 0.11f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            g->screen = SCREEN_MENU;
            return;
        }
    }

    if (g->screen == SCREEN_UPGRADE)
    {
        float uiScale = (float)g->width / 1280.0f;
        float hScale = (float)g->height / 720.0f;
        float centerX = g->width * 0.5f;
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
        cardY = g->height * 0.36f;

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
                melhorias_escolher(g, i);
                return;
            }
        }

        bx = rerollX;
        by = rerollY;
        if (wx >= bx && wx <= bx + rerollW && wy >= by && wy <= by + rerollH && g->gold >= 3)
        {
            g->gold -= 3;
            melhorias_rolar_opcoes(g);
            interface_desenhar_notificacao(g, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((g->screen == SCREEN_WIN || g->screen == SCREEN_LOSE) && g->nameSaved)
    {
        bx = g->width * 0.5f - 110.0f;
        by = g->height * 0.45f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            g->screen = SCREEN_MENU;
            g->enteringName = 0;
        }
    }
}

void jogo_mouse_movido(Game *g, int x, int y)
{
    g->input.mouseX = x;
    g->input.mouseY = y;
}

void jogo_renderizar(Game *g)
{
    int i;

    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderizar_perspectiva_ortografica(g);
    cenario_desenhar_fundo(g);

    if (g->screen == SCREEN_MENU)
    {
        static float hoverAnim[3] = {0.0f, 0.0f, 0.0f};
        float centerX = g->width * 0.5f;
        float uiScale = (float)g->width / 1280.0f;
        float hScale = (float)g->height / 720.0f;
        float btnW;
        float startH;
        float subH;
        float shadowPad;
        float bx;
        float by;
        float obx;
        float oby;
        float lbx;
        float lby;
        float mx = (float)g->input.mouseX;
        float my = (float)g->input.mouseY;
        float pulse = 0.5f + 0.5f * sinf(g->elapsed * 6.0f);
        int hoverStart;
        int hoverOptions;
        int hoverScores;
        int compactUI;
        int idx;
        int s;

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
        compactUI = (g->width < 1100 || g->height < 680);

        btnW = 300.0f * uiScale;
        startH = 60.0f * uiScale;
        subH = 50.0f * uiScale;
        shadowPad = 10.0f * uiScale;

        bx = centerX - btnW * 0.5f;
        by = g->height * 0.30f;
        obx = centerX - btnW * 0.5f;
        oby = g->height * 0.40f;
        lbx = centerX - btnW * 0.5f;
        lby = g->height * 0.50f;

        hoverStart = (mx >= bx && mx <= bx + btnW && my >= by && my <= by + startH);
        hoverOptions = (mx >= obx && mx <= obx + btnW && my >= oby && my <= oby + subH);
        hoverScores = (mx >= lbx && mx <= lbx + btnW && my >= lby && my <= lby + subH);

        {
            float targets[3];
            targets[0] = hoverStart ? 1.0f : 0.0f;
            targets[1] = hoverOptions ? 1.0f : 0.0f;
            targets[2] = hoverScores ? 1.0f : 0.0f;
            for (idx = 0; idx < 3; ++idx)
            {
                hoverAnim[idx] += (targets[idx] - hoverAnim[idx]) * (8.0f * g->deltaTime);
                if (hoverAnim[idx] < 0.0f)
                {
                    hoverAnim[idx] = 0.0f;
                }
                if (hoverAnim[idx] > 1.0f)
                {
                    hoverAnim[idx] = 1.0f;
                }
            }
        }

        renderizar_texto(centerX - 160.0f, g->height * 0.14f, "ORBIT SIEGE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.98f, 0.95f);
        renderizar_texto(centerX - 310.0f, g->height * 0.20f, "A platformer action game with upgrades and survival mechanics", GLUT_BITMAP_HELVETICA_12, 0.75f, 0.82f, 0.95f);
        if (!compactUI)
        {
            renderizar_texto(centerX - 138.0f, g->height * 0.24f, "Press O for options or L for leaderboard", GLUT_BITMAP_HELVETICA_12, 0.72f, 0.80f, 0.92f);
        }

        if (hoverAnim[0] > 0.01f)
        {
            renderizar_retangulo(bx - shadowPad - 4.0f * uiScale, by - 9.0f * uiScale,
                        btnW + (shadowPad + 4.0f * uiScale) * 2.0f, startH + 18.0f * uiScale,
                        (Color){0.45f, 0.75f, 1.0f, (0.10f + 0.18f * pulse) * hoverAnim[0]});
        }
        renderizar_retangulo(bx - shadowPad, by - 5.0f * uiScale, btnW + shadowPad * 2.0f, startH + 10.0f * uiScale,
                    (Color){0.08f + 0.12f * hoverAnim[0], 0.25f + 0.20f * hoverAnim[0], 0.55f + 0.35f * hoverAnim[0], 0.50f + 0.05f * hoverAnim[0]});
        renderizar_retangulo(bx, by, btnW, startH,
                    (Color){0.15f + 0.07f * hoverAnim[0], 0.45f + 0.13f * hoverAnim[0], 0.85f + 0.15f * hoverAnim[0], 0.95f + 0.03f * hoverAnim[0]});
        renderizar_texto(centerX - 52.0f * uiScale, by + 20.0f * uiScale, "START GAME", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
        renderizar_texto(centerX - 89.0f * uiScale, by + 40.0f * uiScale, "Press ENTER or click", GLUT_BITMAP_HELVETICA_10, 0.75f, 0.85f, 1.0f);

        if (hoverAnim[1] > 0.01f)
        {
            renderizar_retangulo(obx - shadowPad - 3.0f * uiScale, oby - 8.0f * uiScale,
                        btnW + (shadowPad + 3.0f * uiScale) * 2.0f, subH + 16.0f * uiScale,
                        (Color){0.78f, 0.52f, 1.0f, (0.08f + 0.16f * pulse) * hoverAnim[1]});
        }
        renderizar_retangulo(obx - shadowPad, oby - 5.0f * uiScale, btnW + shadowPad * 2.0f, subH + 10.0f * uiScale,
                    (Color){0.26f + 0.15f * hoverAnim[1], 0.12f + 0.07f * hoverAnim[1], 0.36f + 0.24f * hoverAnim[1], 0.50f + 0.05f * hoverAnim[1]});
        renderizar_retangulo(obx, oby, btnW, subH,
                    (Color){0.36f + 0.12f * hoverAnim[1], 0.22f + 0.10f * hoverAnim[1], 0.62f + 0.20f * hoverAnim[1], 0.90f + 0.05f * hoverAnim[1]});
        renderizar_texto(centerX - 52.0f * uiScale, oby + 17.0f * uiScale, "OPTIONS (O)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (hoverAnim[2] > 0.01f)
        {
            renderizar_retangulo(lbx - shadowPad - 3.0f * uiScale, lby - 8.0f * uiScale,
                        btnW + (shadowPad + 3.0f * uiScale) * 2.0f, subH + 16.0f * uiScale,
                        (Color){0.45f, 0.95f, 0.90f, (0.08f + 0.16f * pulse) * hoverAnim[2]});
        }
        renderizar_retangulo(lbx - shadowPad, lby - 5.0f * uiScale, btnW + shadowPad * 2.0f, subH + 10.0f * uiScale,
                    (Color){0.08f + 0.08f * hoverAnim[2], 0.22f + 0.18f * hoverAnim[2], 0.30f + 0.20f * hoverAnim[2], 0.50f + 0.05f * hoverAnim[2]});
        renderizar_retangulo(lbx, lby, btnW, subH,
                    (Color){0.16f + 0.10f * hoverAnim[2], 0.36f + 0.16f * hoverAnim[2], 0.50f + 0.18f * hoverAnim[2], 0.90f + 0.05f * hoverAnim[2]});
        renderizar_texto(centerX - 77.0f * uiScale, lby + 17.0f * uiScale, "LEADERBOARD (L)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (hoverStart || hoverOptions || hoverScores)
        {
            renderizar_texto(centerX - 56.0f * uiScale, g->height * 0.57f, "Click to select", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.96f, 1.0f);
        }

        renderizar_retangulo(g->width * 0.5f - 400.0f, g->height * 0.90f, 800.0f, compactUI ? 40.0f : 60.0f, (Color){0.0f, 0.0f, 0.0f, 0.25f});
        renderizar_texto(g->width * 0.5f - 380.0f, g->height * 0.925f, "A/D or ARROWS: Move  |  SPACE or W: Jump  |  Mouse: Aim  |  LClick: Shoot", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);
        if (!compactUI)
        {
            renderizar_texto(g->width * 0.5f - 380.0f, g->height * 0.95f, "P or ESC: Pause  |  F12: Screenshot  |  Enemy types: Orange (Standard), Diamond (Sniper), Heavy (Tank)", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);
        }

        renderizar_retangulo(30.0f, g->height * 0.78f, 320.0f, 90.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
        {
            char stats[128];
            snprintf(stats, sizeof(stats), "High Score: %d", g->highScore);
            renderizar_texto(50.0f, g->height * 0.82f, stats, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.4f);
            snprintf(stats, sizeof(stats), "Max Wave: %d", g->maxWaveEver);
            renderizar_texto(50.0f, g->height * 0.86f, stats, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.95f, 0.5f);
        }

        renderizar_retangulo(g->width - 350.0f, g->height * 0.78f, 320.0f, 90.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
        {
            char cfg[128];
            snprintf(cfg, sizeof(cfg), "Difficulty: %s", inimigo_nome_dificuldade(g->difficulty));
            renderizar_texto(g->width - 330.0f, g->height * 0.82f, cfg, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.88f, 0.3f);
            snprintf(cfg, sizeof(cfg), "Audio: %s", g->audioEnabled ? "ON" : "OFF");
            renderizar_texto(g->width - 330.0f, g->height * 0.86f, cfg, GLUT_BITMAP_HELVETICA_18, 0.3f, g->audioEnabled ? 0.9f : 0.5f, 0.8f);
        }

        renderizar_retangulo(g->width * 0.5f - 280.0f, g->height * 0.60f, 560.0f, 140.0f, (Color){0.05f, 0.08f, 0.15f, 0.7f});
        renderizar_texto(g->width * 0.5f - 150.0f, g->height * 0.63f, "TOP 5 SCORES", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.9f, 0.5f);
        for (s = 0; s < g->topScoreCount && s < 5; ++s)
        {
            char row[128];
            snprintf(row, sizeof(row), "%d. %-18s  %7d  W%d", s + 1, g->topScores[s].name, g->topScores[s].score, g->topScores[s].wave);
            renderizar_texto(g->width * 0.5f - 260.0f, g->height * 0.67f + s * 20.0f, row, GLUT_BITMAP_HELVETICA_10, 0.8f, 0.88f, 0.98f);
        }
    }

    if (g->screen == SCREEN_SCORES)
    {
        static float scoreHover[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        float centerX = g->width * 0.5f;
        float mx = (float)g->input.mouseX;
        float my = (float)g->input.mouseY;
        float pulse = 0.5f + 0.5f * sinf(g->elapsed * 6.0f);
        int hoverPrev;
        int hoverNext;
        int hoverClear;
        int hoverBack;
        int hi;
        int pageCount = (g->allScoreCount + g->scorePageSize - 1) / g->scorePageSize;
        int startIndex = g->scorePage * g->scorePageSize;
        int endIndex = startIndex + g->scorePageSize;
        char pageText[64];

        hoverPrev = (mx >= centerX - 255.0f && mx <= centerX - 95.0f && my >= g->height * 0.20f && my <= g->height * 0.20f + 44.0f);
        hoverNext = (mx >= centerX + 95.0f && mx <= centerX + 255.0f && my >= g->height * 0.20f && my <= g->height * 0.20f + 44.0f);
        hoverClear = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= g->height * 0.20f && my <= g->height * 0.20f + 44.0f);
        hoverBack = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= g->height * 0.11f && my <= g->height * 0.11f + 44.0f);

        {
            float targets[4];
            targets[0] = hoverPrev ? 1.0f : 0.0f;
            targets[1] = hoverNext ? 1.0f : 0.0f;
            targets[2] = hoverClear ? 1.0f : 0.0f;
            targets[3] = hoverBack ? 1.0f : 0.0f;
            for (hi = 0; hi < 4; ++hi)
            {
                scoreHover[hi] += (targets[hi] - scoreHover[hi]) * (8.0f * g->deltaTime);
                if (scoreHover[hi] < 0.0f)
                    scoreHover[hi] = 0.0f;
                if (scoreHover[hi] > 1.0f)
                    scoreHover[hi] = 1.0f;
            }
        }

        if (pageCount <= 0)
        {
            pageCount = 1;
        }
        if (endIndex > g->allScoreCount)
        {
            endIndex = g->allScoreCount;
        }

        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.0f, 0.0f, 0.0f, 0.67f});
        renderizar_texto(g->width * 0.5f - 78.0f, g->height * 0.82f, "SCOREBOARD", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

        renderizar_retangulo(g->width * 0.5f - 300.0f, g->height * 0.26f, 600.0f, 360.0f, (Color){0.06f, 0.09f, 0.17f, 0.88f});
        renderizar_texto(g->width * 0.5f - 270.0f, g->height * 0.58f, "#   NAME                    SCORE       WAVE", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

        if (g->allScoreCount == 0)
        {
            renderizar_texto(g->width * 0.5f - 96.0f, g->height * 0.45f, "No scores saved yet", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.7f);
        }
        else
        {
            int i;
            for (i = startIndex; i < endIndex; ++i)
            {
                char row[128];
                int rowY = (int)(g->height * 0.53f - (i - startIndex) * 28.0f);
                snprintf(row, sizeof(row), "%02d  %-22s %8d   %4d", i + 1, g->allScores[i].name, g->allScores[i].score, g->allScores[i].wave);
                renderizar_texto(g->width * 0.5f - 270.0f, (float)rowY, row, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.93f, 1.0f);
            }
        }

        snprintf(pageText, sizeof(pageText), "Page %d/%d", g->scorePage + 1, pageCount);
        renderizar_texto(g->width * 0.5f - 40.0f, g->height * 0.22f, pageText, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);

        if (scoreHover[0] > 0.01f)
        {
            renderizar_retangulo(centerX - 258.0f, g->height * 0.20f - 3.0f, 166.0f, 50.0f, (Color){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[0]});
        }
        renderizar_retangulo(g->width * 0.5f - 255.0f, g->height * 0.20f, 160.0f, 44.0f,
                    (Color){0.20f + 0.08f * scoreHover[0], 0.30f + 0.10f * scoreHover[0], 0.52f + 0.16f * scoreHover[0], 0.90f + 0.05f * scoreHover[0]});
        renderizar_texto(g->width * 0.5f - 205.0f, g->height * 0.228f, "PREV", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (scoreHover[1] > 0.01f)
        {
            renderizar_retangulo(centerX + 92.0f, g->height * 0.20f - 3.0f, 166.0f, 50.0f, (Color){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[1]});
        }
        renderizar_retangulo(g->width * 0.5f + 95.0f, g->height * 0.20f, 160.0f, 44.0f,
                    (Color){0.20f + 0.08f * scoreHover[1], 0.30f + 0.10f * scoreHover[1], 0.52f + 0.16f * scoreHover[1], 0.90f + 0.05f * scoreHover[1]});
        renderizar_texto(g->width * 0.5f + 147.0f, g->height * 0.228f, "NEXT", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (scoreHover[2] > 0.01f)
        {
            renderizar_retangulo(centerX - 113.0f, g->height * 0.20f - 3.0f, 226.0f, 50.0f, (Color){1.0f, 0.52f, 0.54f, (0.10f + 0.12f * pulse) * scoreHover[2]});
        }
        renderizar_retangulo(g->width * 0.5f - 110.0f, g->height * 0.20f, 220.0f, 44.0f,
                    (Color){0.45f + 0.10f * scoreHover[2], 0.20f + 0.06f * scoreHover[2], 0.22f + 0.08f * scoreHover[2], 0.90f + 0.05f * scoreHover[2]});
        renderizar_texto(g->width * 0.5f - 78.0f, g->height * 0.228f, "CLEAR (C)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (scoreHover[3] > 0.01f)
        {
            renderizar_retangulo(centerX - 113.0f, g->height * 0.11f - 3.0f, 226.0f, 50.0f, (Color){0.52f, 0.78f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[3]});
        }
        renderizar_retangulo(g->width * 0.5f - 110.0f, g->height * 0.11f, 220.0f, 44.0f,
                    (Color){0.10f + 0.08f * scoreHover[3], 0.35f + 0.12f * scoreHover[3], 0.70f + 0.14f * scoreHover[3], 0.90f + 0.05f * scoreHover[3]});
        renderizar_texto(g->width * 0.5f - 82.0f, g->height * 0.138f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    if (g->screen == SCREEN_OPTIONS)
    {
        static float optHover[3] = {0.0f, 0.0f, 0.0f};
        float centerX = g->width * 0.5f;
        float mx = (float)g->input.mouseX;
        float my = (float)g->input.mouseY;
        float pulse = 0.5f + 0.5f * sinf(g->elapsed * 6.0f);
        int hoverAudio = (mx >= centerX - 180.0f && mx <= centerX + 180.0f && my >= g->height * 0.56f && my <= g->height * 0.56f + 56.0f);
        int hoverDiff = (mx >= centerX - 180.0f && mx <= centerX + 180.0f && my >= g->height * 0.46f && my <= g->height * 0.46f + 56.0f);
        int hoverBack = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= g->height * 0.31f && my <= g->height * 0.31f + 50.0f);
        int oi;
        char line[128];

        {
            float targets[3];
            targets[0] = hoverAudio ? 1.0f : 0.0f;
            targets[1] = hoverDiff ? 1.0f : 0.0f;
            targets[2] = hoverBack ? 1.0f : 0.0f;
            for (oi = 0; oi < 3; ++oi)
            {
                optHover[oi] += (targets[oi] - optHover[oi]) * (8.0f * g->deltaTime);
                if (optHover[oi] < 0.0f)
                    optHover[oi] = 0.0f;
                if (optHover[oi] > 1.0f)
                    optHover[oi] = 1.0f;
            }
        }

        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.0f, 0.0f, 0.0f, 0.62f});
        renderizar_texto(g->width * 0.5f - 76.0f, g->height * 0.73f, "OPTIONS", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

        if (optHover[0] > 0.01f)
        {
            renderizar_retangulo(centerX - 183.0f, g->height * 0.56f - 3.0f, 366.0f, 62.0f, (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * optHover[0]});
        }
        renderizar_retangulo(g->width * 0.5f - 180.0f, g->height * 0.56f, 360.0f, 56.0f,
                    (Color){0.20f + 0.08f * optHover[0], 0.25f + 0.10f * optHover[0], 0.50f + 0.16f * optHover[0], 0.90f + 0.05f * optHover[0]});
        snprintf(line, sizeof(line), "Audio: %s  (click or A)", g->audioEnabled ? "ON" : "OFF");
        renderizar_texto(g->width * 0.5f - 150.0f, g->height * 0.595f, line, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        if (optHover[1] > 0.01f)
        {
            renderizar_retangulo(centerX - 183.0f, g->height * 0.46f - 3.0f, 366.0f, 62.0f, (Color){0.84f, 0.66f, 1.0f, (0.10f + 0.12f * pulse) * optHover[1]});
        }
        renderizar_retangulo(g->width * 0.5f - 180.0f, g->height * 0.46f, 360.0f, 56.0f,
                    (Color){0.28f + 0.10f * optHover[1], 0.22f + 0.08f * optHover[1], 0.44f + 0.15f * optHover[1], 0.90f + 0.05f * optHover[1]});
        snprintf(line, sizeof(line), "Difficulty: %s  (click or D)", inimigo_nome_dificuldade(g->difficulty));
        renderizar_texto(g->width * 0.5f - 150.0f, g->height * 0.495f, line, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        renderizar_texto(g->width * 0.5f - 180.0f, g->height * 0.40f, "Easy: less damage + more time | Hard: stronger/faster enemies", GLUT_BITMAP_HELVETICA_12, 0.92f, 0.92f, 0.92f);

        if (optHover[2] > 0.01f)
        {
            renderizar_retangulo(centerX - 113.0f, g->height * 0.31f - 3.0f, 226.0f, 56.0f, (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * optHover[2]});
        }
        renderizar_retangulo(g->width * 0.5f - 110.0f, g->height * 0.31f, 220.0f, 50.0f,
                    (Color){0.10f + 0.08f * optHover[2], 0.35f + 0.12f * optHover[2], 0.70f + 0.14f * optHover[2], 0.90f + 0.05f * optHover[2]});
        renderizar_texto(g->width * 0.5f - 76.0f, g->height * 0.342f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    if (g->screen == SCREEN_PLAYING || g->screen == SCREEN_PAUSED || g->screen == SCREEN_UPGRADE || g->screen == SCREEN_WIN || g->screen == SCREEN_LOSE)
    {
        int i;
        cenario_desenhar_plataformas(g);
        for (i = 0; i < MAXIMO_INIMIGOS; ++i)
        {
            if (g->enemies[i].active)
            {
                interface_desenhar_inimigo(&g->enemies[i]);
            }
        }

        projeteis_desenhar(g);
        particulas_desenhar(g);
        interface_desenhar_jogador(g);
        interface_desenhar_hud(g);
        interface_desenhar_vida_boss(g);
        if (g->screen == SCREEN_PLAYING)
        {
            interface_desenhar_mira(g);
        }
    }

    if (g->screen == SCREEN_PAUSED)
    {
        float centerX = g->width * 0.5f;
        int compactUI = (g->width < 1100 || g->height < 680);
        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.0f, 0.0f, 0.0f, 0.56f});
        renderizar_retangulo(centerX - 240.0f, g->height * 0.42f, 480.0f, compactUI ? 110.0f : 140.0f, (Color){0.06f, 0.10f, 0.18f, 0.88f});
        renderizar_texto(centerX - 64.0f, g->height * 0.47f, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
        renderizar_texto(centerX - 150.0f, g->height * 0.53f, "Press P or ENTER to continue", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
        if (!compactUI)
        {
            renderizar_texto(centerX - 120.0f, g->height * 0.58f, "ESC also resumes the game", GLUT_BITMAP_HELVETICA_12, 0.78f, 0.86f, 0.98f);
        }
    }

    if (g->screen == SCREEN_UPGRADE)
    {
        static float upgradeHoverAnim[MAXIMO_OPCOES_UPGRADE] = {0.0f, 0.0f, 0.0f};
        static float rerollHoverAnim = 0.0f;
        Vetor2D mouse = matematica_vetor2d((float)g->input.mouseX, (float)g->input.mouseY);
        float centerX = g->width * 0.5f;
        float uiScale = (float)g->width / 1280.0f;
        float hScale = (float)g->height / 720.0f;
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
        float pulse = 0.5f + 0.5f * sinf(g->elapsed * 6.0f);
        int compactUI;

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
        compactUI = (g->width < 1100 || g->height < 680);

        cardW = 170.0f * uiScale;
        cardH = 130.0f * uiScale;
        cardGap = 20.0f * uiScale;
        totalW = cardW * MAXIMO_OPCOES_UPGRADE + cardGap * (MAXIMO_OPCOES_UPGRADE - 1);
        startX = centerX - totalW * 0.5f;
        cardY = g->height * 0.36f;

        rerollW = 220.0f * uiScale;
        rerollH = 44.0f * uiScale;
        rerollX = centerX - rerollW * 0.5f;
        rerollY = cardY + cardH + 20.0f * uiScale;

        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.0f, 0.0f, 0.0f, 0.58f});
        renderizar_retangulo(centerX - totalW * 0.5f - 40.0f * uiScale, cardY - 70.0f * uiScale,
                    totalW + 80.0f * uiScale, cardH + 190.0f * uiScale, (Color){0.07f, 0.10f, 0.18f, 0.86f});
        renderizar_texto(centerX - 92.0f, cardY - 34.0f * uiScale, "CHOOSE UPGRADE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
        if (!compactUI)
        {
            renderizar_texto(centerX - 155.0f, cardY - 12.0f * uiScale, "Select with mouse click or keys 1, 2, 3", GLUT_BITMAP_HELVETICA_12, 0.80f, 0.88f, 1.0f);
        }
        g->upgradeHover = -1;

        for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
        {
            float bx = startX + i * (cardW + cardGap);
            float by = cardY;
            int hovered = (mouse.x >= bx && mouse.x <= bx + cardW && mouse.y >= by && mouse.y <= by + cardH);
            if (hovered)
            {
                g->upgradeHover = i;
            }
            upgradeHoverAnim[i] += ((hovered ? 1.0f : 0.0f) - upgradeHoverAnim[i]) * (8.0f * g->deltaTime);
            if (upgradeHoverAnim[i] < 0.0f)
            {
                upgradeHoverAnim[i] = 0.0f;
            }
            if (upgradeHoverAnim[i] > 1.0f)
            {
                upgradeHoverAnim[i] = 1.0f;
            }

            if (upgradeHoverAnim[i] > 0.01f)
            {
                renderizar_retangulo(bx - 4.0f * uiScale, by - 4.0f * uiScale, cardW + 8.0f * uiScale, cardH + 8.0f * uiScale,
                            (Color){0.65f, 0.88f, 1.0f, (0.10f + 0.14f * pulse) * upgradeHoverAnim[i]});
            }
            renderizar_retangulo(bx, by, cardW, cardH,
                        (Color){0.14f + 0.06f * upgradeHoverAnim[i], 0.20f + i * 0.07f + 0.06f * upgradeHoverAnim[i], 0.45f + 0.10f * upgradeHoverAnim[i], 0.92f + 0.04f * upgradeHoverAnim[i]});
            melhorias_desenhar_icone(g->upgrades[i].type, bx + cardW - 25.0f * uiScale, by + cardH - 26.0f * uiScale);
            renderizar_texto(bx + 12.0f * uiScale, by + 34.0f * uiScale, g->upgrades[i].label, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
            renderizar_texto_limites(bx + 12.0f * uiScale, by + 62.0f * uiScale, g->upgrades[i].desc, GLUT_BITMAP_HELVETICA_12, 0.88f, 0.95f, 1.0f, cardW - 32.0f * uiScale);
            {
                char idx[8];
                snprintf(idx, sizeof(idx), "[%d]", i + 1);
                renderizar_texto(bx + 12.0f * uiScale, by + cardH - 14.0f * uiScale, idx, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.95f, 0.6f);
            }
        }

        {
            int rerollHovered = (mouse.x >= rerollX && mouse.x <= rerollX + rerollW && mouse.y >= rerollY && mouse.y <= rerollY + rerollH);
            rerollHoverAnim += ((rerollHovered ? 1.0f : 0.0f) - rerollHoverAnim) * (8.0f * g->deltaTime);
            if (rerollHoverAnim < 0.0f)
            {
                rerollHoverAnim = 0.0f;
            }
            if (rerollHoverAnim > 1.0f)
            {
                rerollHoverAnim = 1.0f;
            }

            if (rerollHoverAnim > 0.01f)
            {
                renderizar_retangulo(rerollX - 3.0f * uiScale, rerollY - 3.0f * uiScale, rerollW + 6.0f * uiScale, rerollH + 6.0f * uiScale,
                            (Color){0.75f, 0.64f, 1.0f, (0.08f + 0.12f * pulse) * rerollHoverAnim});
            }
            renderizar_retangulo(rerollX, rerollY, rerollW, rerollH,
                        (Color){0.22f + 0.10f * rerollHoverAnim, 0.18f + 0.08f * rerollHoverAnim, 0.35f + 0.12f * rerollHoverAnim, 0.92f + 0.04f * rerollHoverAnim});
            renderizar_texto(centerX - 90.0f * uiScale, rerollY + 26.0f * uiScale, "Reroll (R) - Cost: 3 gold", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.95f, 0.9f);
        }

        if (g->upgradeHover >= 0 && g->upgradeHover < MAXIMO_OPCOES_UPGRADE)
        {
            renderizar_retangulo(centerX - 240.0f * uiScale, rerollY + rerollH + 12.0f * uiScale, 480.0f * uiScale, 40.0f * uiScale, (Color){0.08f, 0.12f, 0.24f, 0.85f});
            renderizar_texto(centerX - 220.0f * uiScale, rerollY + rerollH + 36.0f * uiScale, g->upgrades[g->upgradeHover].desc, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);
        }
    }

    if (g->screen == SCREEN_WIN || g->screen == SCREEN_LOSE)
    {
        static float endMenuHover = 0.0f;
        const char *title = (g->screen == SCREEN_WIN) ? "VICTORY" : "DEFEAT";
        float centerX = g->width * 0.5f;
        float mx = (float)g->input.mouseX;
        float my = (float)g->input.mouseY;
        float pulse = 0.5f + 0.5f * sinf(g->elapsed * 6.0f);
        int hoverMenu = (g->nameSaved && mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= g->height * 0.45f && my <= g->height * 0.45f + 50.0f);
        char line[160];

        endMenuHover += ((hoverMenu ? 1.0f : 0.0f) - endMenuHover) * (8.0f * g->deltaTime);
        if (endMenuHover < 0.0f)
            endMenuHover = 0.0f;
        if (endMenuHover > 1.0f)
            endMenuHover = 1.0f;

        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.0f, 0.0f, 0.0f, 0.62f});
        renderizar_retangulo(centerX - 280.0f, g->height * 0.38f, 560.0f, 260.0f, (Color){0.08f, 0.10f, 0.18f, 0.86f});
        renderizar_texto(centerX - 60.0f, g->height * 0.44f, title, GLUT_BITMAP_TIMES_ROMAN_24,
                    g->screen == SCREEN_WIN ? 0.9f : 1.0f,
                    g->screen == SCREEN_WIN ? 1.0f : 0.85f,
                    g->screen == SCREEN_WIN ? 0.9f : 0.85f);

        snprintf(line, sizeof(line), "Score: %d  |  Wave: %d", g->score, g->wave);
        renderizar_texto(centerX - 100.0f, g->height * 0.50f, line, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

        if (!g->nameSaved)
        {
            renderizar_texto(centerX - 190.0f, g->height * 0.57f, "Type name and press ENTER to save score:", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
            renderizar_retangulo(centerX - 170.0f, g->height * 0.61f, 340.0f, 40.0f, (Color){0.12f, 0.12f, 0.2f, 0.9f});
            renderizar_texto(centerX - 160.0f, g->height * 0.635f, g->playerName, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 0.8f);
        }
        else
        {
            renderizar_texto(centerX - 170.0f, g->height * 0.57f, "Saved. Press ENTER or M to go menu.", GLUT_BITMAP_HELVETICA_18, 0.95f, 1.0f, 0.85f);
            if (endMenuHover > 0.01f)
            {
                renderizar_retangulo(centerX - 113.0f, g->height * 0.45f - 3.0f, 226.0f, 56.0f,
                            (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * endMenuHover});
            }
            renderizar_retangulo(centerX - 110.0f, g->height * 0.45f, 220.0f, 50.0f,
                        (Color){0.10f + 0.10f * endMenuHover, 0.35f + 0.12f * endMenuHover, 0.70f + 0.14f * endMenuHover, 0.90f + 0.05f * endMenuHover});
            renderizar_texto(centerX - 72.0f, g->height * 0.482f, "MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        }
    }

    if (g->screen == SCREEN_PLAYING && g->damageFlash > 0.0f)
    {
        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.9f, 0.1f, 0.1f, 0.18f * g->damageFlash});
    }

    if (g->screen == SCREEN_PLAYING && g->upgradeFlash > 0.0f)
    {
        renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.2f, 0.8f, 1.0f, 0.12f * g->upgradeFlash});
    }

    glutSwapBuffers();
}

void jogo_iniciar_frame(Game *g)
{
    memset(g->input.keysPressed, 0, sizeof(g->input.keysPressed));
    memset(g->input.mousePressed, 0, sizeof(g->input.mousePressed));
}