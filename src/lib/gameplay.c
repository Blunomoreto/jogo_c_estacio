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

static void gameplay_atualizar_jogador_movimento(Jogo *jogo, float delta_tempo)
{
    int oi;
    float moveX = 0.0f;

    if (jogo->entrada.teclas['a'] || jogo->entrada.teclas['A'] || jogo->entrada.especiais[GLUT_KEY_LEFT])
        moveX -= 1.0f;
    if (jogo->entrada.teclas['d'] || jogo->entrada.teclas['D'] || jogo->entrada.especiais[GLUT_KEY_RIGHT])
        moveX += 1.0f;

    jogo->jogador.pos.x += moveX * jogo->jogador.velocidade * delta_tempo;

    if (jogo->jogador.pos.x < jogo->jogador.tamanho)
        jogo->jogador.pos.x = jogo->jogador.tamanho;
    if (jogo->jogador.pos.x > jogo->largura - jogo->jogador.tamanho)
        jogo->jogador.pos.x = jogo->largura - jogo->jogador.tamanho;

    if (jogo->entrada.teclas[' '] || jogo->entrada.teclas['w'] || jogo->entrada.teclas['W'])
    {
        if (jogo->jogador.esta_no_chao)
        {
            jogo->jogador.velocidade_y = -JOGADOR_FORCA_PULO;
            jogo->jogador.esta_no_chao = 0;
            audio_tocar_som_pulo_inicio();
        }
    }

    jogo->jogador.velocidade_y += GRAVIDADE * delta_tempo;
    if (jogo->jogador.velocidade_y > 600.0f)
        jogo->jogador.velocidade_y = 600.0f;
    jogo->jogador.pos.y += jogo->jogador.velocidade_y * delta_tempo;

    {
        static Vetor2D prevP = {0, 0};
        static Vetor2D prevV = {0, 0};
        if (prevP.x == 0 && prevP.y == 0)
            prevP = jogo->jogador.pos;
        jogo->jogador.vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->jogador.pos, prevP), 1.0f / delta_tempo);
        jogo->jogador.aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->jogador.vel, prevV), 1.0f / delta_tempo);
        prevP = jogo->jogador.pos;
        prevV = jogo->jogador.vel;
    }

    jogo->jogador.esta_no_chao = 0;
    if (jogo->jogador.pos.y + jogo->jogador.tamanho >= ALTURA_CHAO)
    {
        if (jogo->jogador.velocidade_y > 100.0f)
            audio_tocar_som_pulo_fim();
        jogo->jogador.pos.y = ALTURA_CHAO - jogo->jogador.tamanho;
        jogo->jogador.velocidade_y = 0.0f;
        jogo->jogador.esta_no_chao = 1;
    }

    if (jogo->jogador.pos.y - jogo->jogador.tamanho <= 20.0f)
    {
        jogo->jogador.pos.y = 20.0f + jogo->jogador.tamanho;
        jogo->jogador.velocidade_y = 0.0f;
    }

    for (oi = 0; oi < MAXIMO_PLATAFORMAS; ++oi)
    {
        Obstaculo *o = &jogo->obstaculos[oi];
        if (!o->ativo)
            continue;
        if (colisao_circulo_vs_retangulo(jogo->jogador.pos, jogo->jogador.tamanho,
                                         matematica_vetor2d(o->x, o->y),
                                         matematica_vetor2d(o->x + o->largura, o->y + o->altura)))
        {
            float cx = o->x + o->largura * 0.5f;
            float cy = o->y + o->altura * 0.5f;
            float dx = fabsf(jogo->jogador.pos.x - cx);
            float dy = fabsf(jogo->jogador.pos.y - cy);

            if (jogo->jogador.pos.y - jogo->jogador.tamanho < cy && jogo->jogador.velocidade_y >= 0.0f)
            {
                if (jogo->jogador.velocidade_y > 50.0f)
                    audio_tocar_som_pulo_fim();
                jogo->jogador.pos.y = o->y - jogo->jogador.tamanho;
                jogo->jogador.velocidade_y = 0.0f;
                jogo->jogador.esta_no_chao = 1;
            }
            else if (jogo->jogador.pos.y - jogo->jogador.tamanho >= cy && dx > dy)
            {
                if (jogo->jogador.pos.x < cx)
                    jogo->jogador.pos.x = o->x - jogo->jogador.tamanho - 5.0f;
                else
                    jogo->jogador.pos.x = o->x + o->largura + jogo->jogador.tamanho + 5.0f;
                if (jogo->jogador.pos.x < jogo->jogador.tamanho)
                    jogo->jogador.pos.x = jogo->jogador.tamanho;
                if (jogo->jogador.pos.x > jogo->largura - jogo->jogador.tamanho)
                    jogo->jogador.pos.x = jogo->largura - jogo->jogador.tamanho;
            }
        }
    }
}

static void gameplay_atirar_jogador(Jogo *jogo, float delta_tempo)
{
    int i;
    jogo->jogador.tempo_recarga_disparo -= delta_tempo;

    if (jogo->entrada.mouse_segurado[0] && jogo->jogador.tempo_recarga_disparo <= 0.0f)
    {
        Vetor2D target = matematica_mouse_para_mundo(jogo);
        Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, jogo->jogador.pos));
        projeteis_criar(jogo, jogo->jogador.pos, dir, 1, jogo->jogador.velocidade_projetil, jogo->jogador.dano, 6.0f, 2.5f, GUIANCA_NENHUMA, -1, 0.0f);
        particulas_criar(jogo, jogo->jogador.pos, 4, (Cor){0.3f, 0.9f, 1.0f, 0.85f});
        jogo->jogador.tempo_recarga_disparo = jogo->jogador.taxa_disparo;
        audio_tocar_som_tiro_disparo();
    }

    if (jogo->entrada.mouse_segurado[2] && jogo->jogador.tempo_recarga_disparo <= 0.0f && jogo->jogador.municao_guiada > 0)
    {
        if (jogo->jogador.tem_guianca_pp || jogo->jogador.tem_guianca_apn)
        {
            Vetor2D target = matematica_mouse_para_mundo(jogo);
            int bestTarget = -1;
            float minD = 1000.0f;
            for (i = 0; i < MAXIMO_INIMIGOS; ++i)
            {
                if (jogo->inimigos[i].ativo)
                {
                    float d = matematica_vetor2d_len(matematica_vetor2d_subtracao(inimigo_posicao(&jogo->inimigos[i]), target));
                    if (d < minD)
                    {
                        minD = d;
                        bestTarget = i;
                    }
                }
            }
            if (bestTarget != -1)
            {
                Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, jogo->jogador.pos));
                TipoGuianca lei = jogo->jogador.tem_guianca_apn ? GUIANCA_APN : GUIANCA_PP;
                projeteis_criar(jogo, jogo->jogador.pos, dir, 1, jogo->jogador.velocidade * 1.3f, jogo->jogador.dano * 3.0f, 8.0f, 5.0f, lei, bestTarget, jogo->jogador.aceleracao_lateral_max);
                jogo->jogador.municao_guiada--;
                jogo->jogador.tempo_recarga_disparo = jogo->jogador.taxa_disparo * 2.0f;
                audio_tocar_som_tiro_disparo();
            }
        }
    }
}

static void gameplay_atualizar_projeteis_internos(Jogo *jogo, float delta_tempo)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &jogo->projetis[i];
        int oi;
        if (!p->ativo)
            continue;

        if (p->guianca != GUIANCA_NENHUMA && p->errou)
        {
            p->temporizador_deteccao -= delta_tempo;
            if (p->temporizador_deteccao <= 0.0f)
                p->ativo = 0;
            p->vida -= delta_tempo;
            p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, delta_tempo));
            continue;
        }

        if (p->guianca != GUIANCA_NENHUMA)
        {
            float speed = matematica_vetor2d_len(p->vel);
            float gamma = atan2f(p->vel.y, p->vel.x);
            float aCmd = 0.0f;
            Vetor2D targetPos, targetVel = {0, 0}, targetAccel = {0, 0};
            int targetValid = 0;

            if (p->vem_do_jogador)
            {
                if (p->indice_alvo != -1 && jogo->inimigos[p->indice_alvo].ativo)
                {
                    Inimigo *e = &jogo->inimigos[p->indice_alvo];
                    targetPos = inimigo_posicao(e);
                    targetVel = e->vel;
                    targetAccel = e->aceleracao;
                    targetValid = 1;
                }
            }
            else
            {
                targetPos = jogo->jogador.pos;
                targetVel = jogo->jogador.vel;
                targetAccel = jogo->jogador.aceleracao;
                targetValid = 1;
            }

            if (targetValid)
            {
                Vetor2D dPos = matematica_vetor2d_subtracao(targetPos, p->pos);
                float dist = matematica_vetor2d_len(dPos);
                float los = atan2f(dPos.y, dPos.x);
                float losRate = (dPos.x * (targetVel.y - p->vel.y) - dPos.y * (targetVel.x - p->vel.x)) / (dist * dist);

                if (p->guianca == GUIANCA_APN && p->distancia_anterior < 9999.0f &&
                    dist > p->distancia_anterior && dist < 250.0f && p->distancia_anterior < 270.0f)
                {
                    p->errou = 1;
                    p->temporizador_deteccao = 0.2f;
                }

                if (p->guianca == GUIANCA_PP && !p->errou && p->distancia_anterior < 9999.0f)
                {
                    if (fabs(losRate) * speed > p->aceleracao_lateral_max && dist < 300.0f)
                    {
                        p->errou = 1;
                        p->temporizador_deteccao = 0.2f;
                    }
                }

                p->distancia_anterior = dist;

                if (!p->errou)
                {
                    if (p->guianca == GUIANCA_APN)
                    {
                        Vetor2D relV = matematica_vetor2d_subtracao(targetVel, p->vel);
                        float Vc = -(dPos.x * relV.x + dPos.y * relV.y) / dist;
                        float losRate_accel = (dPos.x * relV.y - dPos.y * relV.x) / (dist * dist);
                        float a_t_perp = -targetAccel.x * sinf(los) + targetAccel.y * cosf(los);
                        float N = MISSIL_GUIANCA_APN_GANHO;
                        aCmd = N * Vc * losRate_accel + (N * 0.5f) * a_t_perp;
                    }
                    else if (p->guianca == GUIANCA_PP)
                    {
                        float err = los - gamma;
                        while (err > (float)M_PI)
                            err -= 2.0f * (float)M_PI;
                        while (err < -(float)M_PI)
                            err += 2.0f * (float)M_PI;
                        aCmd = MISSIL_GUIANCA_PP_GANHO * speed * err;
                    }

                    if (aCmd > p->aceleracao_lateral_max)
                        aCmd = p->aceleracao_lateral_max;
                    if (aCmd < -p->aceleracao_lateral_max)
                        aCmd = -p->aceleracao_lateral_max;

                    p->aceleracao_lateral += (aCmd - p->aceleracao_lateral) * (delta_tempo / MISSIL_GUIANCA_LAG);
                }
            }

            if (!p->errou)
            {
                float a_x_lateral = -p->aceleracao_lateral * sinf(gamma);
                float a_y_lateral = p->aceleracao_lateral * cosf(gamma);
                p->vel.x += a_x_lateral * delta_tempo;
                p->vel.y += a_y_lateral * delta_tempo;

                float newSpeed = matematica_vetor2d_len(p->vel);
                if (newSpeed > 0.001f)
                    p->vel = matematica_vetor2d_multiplicacao(p->vel, speed / newSpeed);
            }
        }

        p->vida -= delta_tempo;
        p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, delta_tempo));

        if (p->vida <= 0.0f || p->pos.x < -20.0f || p->pos.x > jogo->largura + 20.0f ||
            p->pos.y < -20.0f || p->pos.y > jogo->altura + 20.0f)
        {
            p->ativo = 0;
        }

        for (oi = 0; oi < MAXIMO_PLATAFORMAS && p->ativo; ++oi)
        {
            Obstaculo *o = &jogo->obstaculos[oi];
            if (!o->ativo)
                continue;
            if (colisao_circulo_vs_retangulo(p->pos, p->raio,
                                             matematica_vetor2d(o->x, o->y),
                                             matematica_vetor2d(o->x + o->largura, o->y + o->altura)))
                p->ativo = 0;
        }
    }
}

static int gameplay_atualizar_inimigos_internos(Jogo *jogo, float delta_tempo, float scoreMul)
{
    int i;
    int tookHit = 0;

    for (i = 0; i < MAXIMO_INIMIGOS; ++i)
    {
        Inimigo *e = &jogo->inimigos[i];
        int j;
        Vetor2D ep;
        if (!e->ativo)
            continue;

        e->angulo += e->velocidade_angular * delta_tempo;
        e->flash_dano -= delta_tempo * 4.0f;
        if (e->flash_dano < 0.0f)
            e->flash_dano = 0.0f;

        ep = inimigo_posicao(e);

        {
            static Vetor2D prevEnemyPos[MAXIMO_INIMIGOS] = {{0, 0}};
            static Vetor2D prevEnemyVel[MAXIMO_INIMIGOS] = {{0, 0}};
            if (prevEnemyPos[i].x == 0 && prevEnemyPos[i].y == 0)
                prevEnemyPos[i] = ep;
            e->vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(ep, prevEnemyPos[i]), 1.0f / delta_tempo);
            e->aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(e->vel, prevEnemyVel[i]), 1.0f / delta_tempo);
            prevEnemyPos[i] = ep;
            prevEnemyVel[i] = e->vel;
        }

        if (colisao_circulo_vs_circulo(jogo->jogador.pos, jogo->jogador.tamanho * 0.8f, ep, e->tamanho))
        {
            jogo->jogador.vida = matematica_limite_min(jogo->jogador.vida -= e->dano * delta_tempo, 0);
            tookHit = 1;
            particulas_criar(jogo, jogo->jogador.pos, 1, (Cor){1.0f, 0.2f, 0.2f, 0.8f});
        }

        e->tempo_recarga_disparo -= delta_tempo;
        if (e->tempo_recarga_disparo <= 0.0f)
        {
            Vetor2D dirToPlayer = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(jogo->jogador.pos, ep));
            float enemyShotSpeed;
            float enemyShotDamage;

            if (e->tipo == INIMIGO_DIAMANTE)
            {
                if (e->contador_rajada > 0)
                {
                    float ppOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 0.5f;
                    projeteis_criar(jogo, ep, dirToPlayer, 0, 200.0f + jogo->onda * 10.0f, 25.0f + jogo->onda * 4.0f, 6.0f, 4.0f, GUIANCA_PP, -1, ppOverload);
                    e->contador_rajada--;
                    e->tempo_recarga_disparo = 0.5f;
                    if (e->contador_rajada == 0)
                    {
                        e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                        e->contador_rajada = 2;
                    }
                }
                else
                {
                    e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                    e->contador_rajada = 2;
                }
            }
            else if (e->tipo == INIMIGO_PENTAGONO)
            {
                float apnOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 1.0f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, 300.0f + jogo->onda * 15.0f, 30.0f + jogo->onda * 5.0f, 6.0f, 6.0f, GUIANCA_APN, -1, apnOverload);
                e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(2.5f, 4.0f);
            }
            else if (e->eh_chefao)
            {
                Vetor2D sideA = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f - dirToPlayer.y * 0.38f, dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                Vetor2D sideB = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f + dirToPlayer.y * 0.38f, -dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                enemyShotSpeed = 260.0f + jogo->onda * 18.0f;
                enemyShotDamage = 9.5f + jogo->onda * 1.4f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 8.0f, 4.4f, GUIANCA_NENHUMA, -1, 0.0f);
                projeteis_criar(jogo, ep, sideA, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                projeteis_criar(jogo, ep, sideB, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.8f, 1.8f) - jogo->onda * 0.05f;
            }
            else if (e->tipo == INIMIGO_ATIRADOR)
            {
                enemyShotSpeed = 330.0f + jogo->onda * 22.0f;
                enemyShotDamage = 9.0f + jogo->onda * 1.6f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 5.5f, 3.6f, GUIANCA_NENHUMA, -1, 0.0f);
                e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.0f, 2.1f) - jogo->onda * 0.04f;
            }
            else if (e->tipo == INIMIGO_TANQUE)
            {
                enemyShotSpeed = 180.0f + jogo->onda * 14.0f;
                enemyShotDamage = 11.0f + jogo->onda * 1.8f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 9.0f, 4.8f, GUIANCA_NENHUMA, -1, 0.0f);
                e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.8f, 3.2f) - jogo->onda * 0.03f;
            }
            else
            {
                enemyShotSpeed = 220.0f + jogo->onda * 18.0f;
                enemyShotDamage = 7.0f + jogo->onda * 1.4f;
                projeteis_criar(jogo, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.1f, 2.6f) - jogo->onda * 0.05f;
            }

            if (e->tempo_recarga_disparo < 0.45f)
                e->tempo_recarga_disparo = 0.45f;
        }

        for (j = 0; j < MAXIMO_PROJETEIS; ++j)
        {
            Projetil *p = &jogo->projetis[j];
            if (!p->ativo || !p->vem_do_jogador)
                continue;

            if (colisao_circulo_vs_circulo(p->pos, p->raio, ep, e->tamanho))
            {
                p->ativo = 0;
                e->vida = matematica_limite_min(e->vida -= p->dano, 0);
                e->flash_dano = 1.0f;
                particulas_criar(jogo, ep, 8, (Cor){1.0f, 0.6f, 0.2f, 0.95f});
                audio_tocar_som_tiro_atingido();

                if (e->vida <= 0.0f)
                {
                    e->ativo = 0;
                    jogo->inimigos_restantes--;
                    if (e->eh_chefao)
                    {
                        jogo->pontuacao += (int)(1500.0f * scoreMul);
                        jogo->ouro += 6;
                    }
                    else if (e->tipo == INIMIGO_TANQUE)
                    {
                        jogo->pontuacao += (int)((230 + jogo->onda * 28) * scoreMul);
                        jogo->ouro += 2;
                    }
                    else if (e->tipo == INIMIGO_ATIRADOR)
                    {
                        jogo->pontuacao += (int)((150 + jogo->onda * 24) * scoreMul);
                        jogo->ouro += 1;
                    }
                    else
                    {
                        jogo->pontuacao += (int)((120 + jogo->onda * 20) * scoreMul);
                        jogo->ouro += 1;
                    }
                    particulas_criar(jogo, ep, 18, (Cor){1.0f, 0.85f, 0.2f, 0.95f});
                }
                break;
            }
        }
    }
    return tookHit;
}

static int gameplay_verificar_projeteis_vs_jogador(Jogo *jogo)
{
    int i;
    int tookHit = 0;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &jogo->projetis[i];
        if (!p->ativo || p->vem_do_jogador)
            continue;

        if (colisao_circulo_vs_circulo(p->pos, p->raio, jogo->jogador.pos, jogo->jogador.tamanho * 0.75f))
        {
            p->ativo = 0;
            jogo->jogador.vida = matematica_limite_min(jogo->jogador.vida -= p->dano, 0);
            tookHit = 1;
            particulas_criar(jogo, jogo->jogador.pos, 7, (Cor){1.0f, 0.3f, 0.2f, 0.9f});
            audio_tocar_som_tiro_atingido();
        }
    }
    return tookHit;
}

static void gameplay_atualizar_particulas_internos(Jogo *jogo, float delta_tempo)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particula *pt = &jogo->particulas[i];
        if (!pt->ativo)
            continue;
        pt->vida -= delta_tempo;
        pt->pos = matematica_vetor2d_adicao(pt->pos, matematica_vetor2d_multiplicacao(pt->vel, delta_tempo));
        pt->vel = matematica_vetor2d_multiplicacao(pt->vel, 0.96f);
        if (pt->vida <= 0.0f)
            pt->ativo = 0;
    }
}

void gameplay_atualizar_playing(Jogo *jogo, float delta_tempo)
{
    int tookHit;
    float scoreMul = inimigo_multiplicador_pontos(jogo->dificuldade);

    gameplay_atualizar_jogador_movimento(jogo, delta_tempo);
    gameplay_atirar_jogador(jogo, delta_tempo);
    gameplay_atualizar_projeteis_internos(jogo, delta_tempo);
    tookHit = gameplay_atualizar_inimigos_internos(jogo, delta_tempo, scoreMul);
    tookHit |= gameplay_verificar_projeteis_vs_jogador(jogo);
    gameplay_atualizar_particulas_internos(jogo, delta_tempo);

    jogo->tempo_decorrido += delta_tempo;
    jogo->tempo_restante -= delta_tempo;
    jogo->pontuacao += (int)(delta_tempo * 14.0f * scoreMul);
    jogo->flash_dano -= delta_tempo * 2.4f;
    if (jogo->flash_dano < 0.0f)
        jogo->flash_dano = 0.0f;
    if (tookHit)
        jogo->flash_dano = 0.9f;

    if (jogo->jogador.vida <= 0.0f || jogo->tempo_restante <= 0.0f)
    {
        interface_encerrar_partida(jogo, TELA_DERROTA);
        return;
    }

    if (jogo->inimigos_restantes <= 0)
    {
        if (jogo->onda >= jogo->ondas_para_vencer)
        {
            interface_encerrar_partida(jogo, TELA_VITORIA);
        }
        else
        {
            jogo->tempo_restante += 6.0f;
            jogo->pontuacao += (int)((180 + jogo->onda * 40) * scoreMul);
            melhorias_rolar_opcoes(jogo);
            jogo->tela = TELA_MELHORIA;
        }
    }
}
