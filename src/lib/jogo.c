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

static void game_reset_player(Jogo *g)
{
    memset(&g->jogador, 0, sizeof(g->jogador));

    g->jogador.pos = matematica_vetor2d((float)g->largura * 0.5f, ALTURA_CHAO - 16.0f);
    g->jogador.tamanho = 16.0f;
    g->jogador.vida = JOGADOR_INICIAL_VIDA;
    g->jogador.vidaMaxima = JOGADOR_INICIAL_VIDA;
    g->jogador.velocidade = JOGADOR_INICIAL_VELOCIDADE;
    g->jogador.dano = JOGADOR_INICIAL_DANO;
    g->jogador.taxaDeDisparo = JOGADOR_INICIAL_TAXA_DISPARO;
    g->jogador.tempoRecargaDisparo = 0.0f;
    g->jogador.velocidadeProjetil = JOGADOR_INICIAL_VELOCIDADE_PROJETIL;
    g->jogador.velocidadeY = 0.0f;
    g->jogador.estaNoChao = 1;
    g->jogador.tempoTeclaPuloPressionada = 0.0f;

    g->jogador.possuiPontoPonto = 0;
    g->jogador.possuiAnguloPonto = 0;
    g->jogador.municaoGuiada = 0;
    g->jogador.municaoGuiadaMaxima = 0;
    g->jogador.aceleracaoLateralMaxima = JOGADOR_INICIAL_ACELERACAO_LATERAL;
}

void jogo_reiniciar(Jogo *g)
{
    game_reset_player(g);
    limpar_entidades(g);

    g->onda = 1;
    g->ondasParaVencer = inimigo_ondas_vitoria(g->dificuldade);
    g->inimigosRestantes = 0;
    g->tempoRestante = inimigo_tempo_inicio(g->dificuldade);
    g->tempoDecorrido = 0.0f;
    g->pontuacao = 0;
    g->ouro = 0;
    g->flashDano = 0.0f;
    g->flashMelhoria = 0.0f;
    g->melhoriaSelecionada = -1;
    g->temporizadorUltimaMelhoria = 0.0f;
    memset(g->ultimaMelhoria, 0, sizeof(g->ultimaMelhoria));
    g->temporizadorToast = 0.0f;
    memset(g->mensagemToast, 0, sizeof(g->mensagemToast));
    g->tela = TELA_JOGANDO;
    g->inserindoNome = 0;
    g->nomeSalvo = 0;

    memset(g->nomeJogador, 0, sizeof(g->nomeJogador));
    snprintf(g->nomeJogador, sizeof(g->nomeJogador), "Player");

    criar_onda(g);
    tocar_musica();
}

void jogo_iniciar(Jogo *g, int width, int height)
{
    memset(g, 0, sizeof(*g));
    srand((unsigned int)time(NULL));

    g->largura = width;
    g->altura = height;
    g->tela = TELA_MENU;
    g->executando = 1;
    g->audioHabilitado = 1;
    g->dificuldade = 1;
    g->paginaPontuacao = 0;
    g->tamanhoPaginaPontuacao = 10;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    persistencia_carregar_stats(&g->maiorPontuacao, &g->maiorOndaAtingida);
    persistencia_carregar_configuracoes(&g->audioHabilitado, &g->dificuldade);
    interface_refrescar_pontuacoes_maximas(g);
    interface_refrescar_pontuacoes(g);

    g->texturaFundo = imagem_carregar_ppm("assets/images/background.ppm", &g->texturaFundoCarregada);
    inicializar_audio();
    definir_audio_ativado(g->audioHabilitado);
}

static void jogo_atualizar_jogando(Jogo *g, float dt)
{
    int i;
    int tookHit = 0;
    float scoreMul = inimigo_multiplicador_pontos(g->dificuldade);

    float moveX = 0.0f;
    if (g->entrada.teclas['a'] || g->entrada.teclas['A'] || g->entrada.especiais[GLUT_KEY_LEFT])
    {
        moveX -= 1.0f;
    }
    if (g->entrada.teclas['d'] || g->entrada.teclas['D'] || g->entrada.especiais[GLUT_KEY_RIGHT])
    {
        moveX += 1.0f;
    }
    g->jogador.pos.x += moveX * g->jogador.velocidade * dt;

    if (g->jogador.pos.x < g->jogador.tamanho)
        g->jogador.pos.x = g->jogador.tamanho;
    if (g->jogador.pos.x > g->largura - g->jogador.tamanho)
        g->jogador.pos.x = g->largura - g->jogador.tamanho;

    if (g->entrada.teclas[' '] || g->entrada.teclas['w'] || g->entrada.teclas['W'])
    {
        if (g->jogador.estaNoChao)
        {
            g->jogador.velocidadeY = -JOGADOR_FORCA_PULO;
            g->jogador.estaNoChao = 0;
            tocar_som_pulo_inicio();
        }
    }

    g->jogador.velocidadeY += GRAVIDADE * dt;

    if (g->jogador.velocidadeY > 600.0f)
        g->jogador.velocidadeY = 600.0f;

    g->jogador.pos.y += g->jogador.velocidadeY * dt;

    {
        static Vetor2D prevP = {0, 0};
        static Vetor2D prevV = {0, 0};
        if (prevP.x == 0 && prevP.y == 0)
            prevP = g->jogador.pos;
        g->jogador.vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(g->jogador.pos, prevP), 1.0f / dt);
        g->jogador.aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(g->jogador.vel, prevV), 1.0f / dt);
        prevP = g->jogador.pos;
        prevV = g->jogador.vel;
    }

    g->jogador.estaNoChao = 0;
    if (g->jogador.pos.y + g->jogador.tamanho >= ALTURA_CHAO)
    {
        if (g->jogador.velocidadeY > 100.0f)
        {
            tocar_som_pulo_fim();
        }
        g->jogador.pos.y = ALTURA_CHAO - g->jogador.tamanho;
        g->jogador.velocidadeY = 0.0f;
        g->jogador.estaNoChao = 1;
    }

    if (g->jogador.pos.y - g->jogador.tamanho <= 20.0f)
    {
        g->jogador.pos.y = 20.0f + g->jogador.tamanho;
        g->jogador.velocidadeY = 0.0f;
    }

    {
        int oi;
        for (oi = 0; oi < MAXIMO_PLATAFORMAS; ++oi)
        {
            Obstaculo *o = &g->obstaculos[oi];
            if (!o->ativo)
            {
                continue;
            }
            if (colisao_circulo_vs_retangulo(g->jogador.pos, g->jogador.tamanho,
                                             matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->largura, o->y + o->altura)))
            {
                float cx = o->x + o->largura * 0.5f;
                float cy = o->y + o->altura * 0.5f;
                float dx = fabsf(g->jogador.pos.x - cx);
                float dy = fabsf(g->jogador.pos.y - cy);

                if (g->jogador.pos.y - g->jogador.tamanho < cy && g->jogador.velocidadeY >= 0.0f)
                {
                    if (g->jogador.velocidadeY > 50.0f)
                    {
                        tocar_som_pulo_fim();
                    }
                    g->jogador.pos.y = o->y - g->jogador.tamanho;
                    g->jogador.velocidadeY = 0.0f;
                    g->jogador.estaNoChao = 1;
                }
                else if (g->jogador.pos.y - g->jogador.tamanho >= cy && dx > dy)
                {
                    if (g->jogador.pos.x < cx)
                    {
                        g->jogador.pos.x = o->x - g->jogador.tamanho - 5.0f;
                    }
                    else
                    {
                        g->jogador.pos.x = o->x + o->largura + g->jogador.tamanho + 5.0f;
                    }
                    if (g->jogador.pos.x < g->jogador.tamanho)
                        g->jogador.pos.x = g->jogador.tamanho;
                    if (g->jogador.pos.x > g->largura - g->jogador.tamanho)
                        g->jogador.pos.x = g->largura - g->jogador.tamanho;
                }
            }
        }
    }

    g->jogador.tempoRecargaDisparo -= dt;

    if (g->entrada.mouseApertado[0] && g->jogador.tempoRecargaDisparo <= 0.0f)
    {
        Vetor2D target = matematica_mouse_para_mundo(g);
        Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, g->jogador.pos));
        projeteis_criar(g, g->jogador.pos, dir, 1, g->jogador.velocidadeProjetil, g->jogador.dano, 6.0f, 2.5f, ORIENTACAO_NENHUMA, -1, 0.0f);
        particulas_criar(g, g->jogador.pos, 4, (Color){0.3f, 0.9f, 1.0f, 0.85f});
        g->jogador.tempoRecargaDisparo = g->jogador.taxaDeDisparo;
        tocar_som_tiro();
    }

    if (g->entrada.mouseApertado[2] && g->jogador.tempoRecargaDisparo <= 0.0f && g->jogador.municaoGuiada > 0)
    {
        if (g->jogador.possuiPontoPonto || g->jogador.possuiAnguloPonto)
        {
            Vetor2D target = matematica_mouse_para_mundo(g);
            int bestTarget = -1;
            float minD = 1000.0f;
            for (i = 0; i < MAXIMO_INIMIGOS; ++i)
            {
                if (g->inimigos[i].ativo)
                {
                    float d = matematica_vetor2d_len(matematica_vetor2d_subtracao(inimigo_posicao(&g->inimigos[i]), target));
                    if (d < minD)
                    {
                        minD = d;
                        bestTarget = i;
                    }
                }
            }
            if (bestTarget != -1)
            {
                Vetor2D dir = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(target, g->jogador.pos));
                TipoOrientacao law = g->jogador.possuiAnguloPonto ? ORIENTACAO_ANGULO_PONTO : ORIENTACAO_PONTO_PONTO;

                projeteis_criar(g, g->jogador.pos, dir, 1, g->jogador.velocidade * 1.3f, g->jogador.dano * 3.0f, 8.0f, 5.0f, law, bestTarget, g->jogador.aceleracaoLateralMaxima);
                g->jogador.municaoGuiada--;
                g->jogador.tempoRecargaDisparo = g->jogador.taxaDeDisparo * 2.0f;
                tocar_som_tiro();
            }
        }
    }

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &g->projetis[i];
        int oi;
        if (!p->ativo)
            continue;

        if (p->orientacao != ORIENTACAO_NENHUMA && p->errou)
        {
            p->temporizadorDeteccao -= dt;
            if (p->temporizadorDeteccao <= 0.0f)
            {
                p->ativo = 0;
            }
            p->vida -= dt;
            p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, dt));
            continue;
        }

        if (p->orientacao != ORIENTACAO_NENHUMA)
        {
            float speed = matematica_vetor2d_len(p->vel);
            float gamma = atan2f(p->vel.y, p->vel.x);
            float aCmd = 0.0f;
            Vetor2D targetPos, targetVel = {0, 0}, targetAccel = {0, 0};
            int targetValid = 0;

            if (p->vemDoJogador)
            {
                if (p->indiceAlvo != -1 && g->inimigos[p->indiceAlvo].ativo)
                {
                    Inimigo *e = &g->inimigos[p->indiceAlvo];
                    targetPos = inimigo_posicao(e);
                    targetVel = e->vel;
                    targetAccel = e->aceleracao;
                    targetValid = 1;
                }
            }
            else
            {
                targetPos = g->jogador.pos;
                targetVel = g->jogador.vel;
                targetAccel = g->jogador.aceleracao;
                targetValid = 1;
            }

            if (targetValid)
            {
                Vetor2D dPos = matematica_vetor2d_subtracao(targetPos, p->pos);
                float dist = matematica_vetor2d_len(dPos);
                float los = atan2f(dPos.y, dPos.x);
                float losRate = (dPos.x * (targetVel.y - p->vel.y) - dPos.y * (targetVel.x - p->vel.x)) / (dist * dist);

                if (p->orientacao == ORIENTACAO_ANGULO_PONTO && p->distanciaAnterior < 9999.0f &&
                    dist > p->distanciaAnterior && dist < 250.0f && p->distanciaAnterior < 270.0f)
                {
                    p->errou = 1;
                    p->temporizadorDeteccao = 0.2f;
                }

                if (p->orientacao == ORIENTACAO_PONTO_PONTO && !p->errou && p->distanciaAnterior < 9999.0f)
                {
                    if (fabs(losRate) * speed > p->aceleracaoLateralMaxima && dist < 300.0f)
                    {
                        p->errou = 1;
                        p->temporizadorDeteccao = 0.2f;
                    }
                }

                p->distanciaAnterior = dist;

                if (!p->errou)
                {
                    if (p->orientacao == ORIENTACAO_ANGULO_PONTO)
                    {
                        Vetor2D relV = matematica_vetor2d_subtracao(targetVel, p->vel);
                        float Vc = -(dPos.x * relV.x + dPos.y * relV.y) / dist;
                        float losRate_accel = (dPos.x * relV.y - dPos.y * relV.x) / (dist * dist);
                        float a_t_perp = -targetAccel.x * sinf(los) + targetAccel.y * cosf(los);

                        float N = MISSIL_GUIANCA_APN_GANHO;
                        aCmd = N * Vc * losRate_accel + (N * 0.5f) * a_t_perp;
                    }
                    else if (p->orientacao == ORIENTACAO_PONTO_PONTO)
                    {
                        float err = los - gamma;
                        while (err > (float)M_PI)
                            err -= 2.0f * (float)M_PI;
                        while (err < -(float)M_PI)
                            err += 2.0f * (float)M_PI;

                        aCmd = MISSIL_GUIANCA_PP_GANHO * speed * err;
                    }

                    if (aCmd > p->aceleracaoLateralMaxima)
                        aCmd = p->aceleracaoLateralMaxima;
                    if (aCmd < -p->aceleracaoLateralMaxima)
                        aCmd = -p->aceleracaoLateralMaxima;

                    p->aceleracaoLateralAtual += (aCmd - p->aceleracaoLateralAtual) * (dt / MISSIL_GUIANCA_LAG);
                }
            }

            if (!p->errou)
            {
                float a_x_lateral = -p->aceleracaoLateralAtual * sinf(gamma);
                float a_y_lateral = p->aceleracaoLateralAtual * cosf(gamma);

                p->vel.x += a_x_lateral * dt;
                p->vel.y += a_y_lateral * dt;

                float newSpeed = matematica_vetor2d_len(p->vel);
                if (newSpeed > 0.001f)
                {
                    p->vel = matematica_vetor2d_multiplicacao(p->vel, speed / newSpeed);
                }
            }
        }

        p->vida -= dt;
        p->pos = matematica_vetor2d_adicao(p->pos, matematica_vetor2d_multiplicacao(p->vel, dt));

        if (p->vida <= 0.0f || p->pos.x < -20.0f || p->pos.x > g->largura + 20.0f || p->pos.y < -20.0f || p->pos.y > g->altura + 20.0f)
        {
            p->ativo = 0;
        }

        for (oi = 0; oi < MAXIMO_PLATAFORMAS && p->ativo; ++oi)
        {
            Obstaculo *o = &g->obstaculos[oi];
            if (!o->ativo)
                continue;
            if (colisao_circulo_vs_retangulo(p->pos, p->raio, matematica_vetor2d(o->x, o->y), matematica_vetor2d(o->x + o->largura, o->y + o->altura)))
                p->ativo = 0;
        }
    }

    for (i = 0; i < MAXIMO_INIMIGOS; ++i)
    {
        Inimigo *e = &g->inimigos[i];
        int j;
        Vetor2D ep;
        if (!e->ativo)
            continue;

        e->angulo += e->velocidadeAngular * dt;
        e->flashDano -= dt * 4.0f;
        if (e->flashDano < 0.0f)
            e->flashDano = 0.0f;

        ep = inimigo_posicao(e);

        {
            static Vetor2D prevInimigoPos[MAXIMO_INIMIGOS] = {{0, 0}};
            static Vetor2D prevInimigoVel[MAXIMO_INIMIGOS] = {{0, 0}};

            if (prevInimigoPos[i].x == 0 && prevInimigoPos[i].y == 0)
            {
                prevInimigoPos[i] = ep;
            }

            e->vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(ep, prevInimigoPos[i]), 1.0f / dt);
            e->aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(e->vel, prevInimigoVel[i]), 1.0f / dt);

            prevInimigoPos[i] = ep;
            prevInimigoVel[i] = e->vel;
        }

        if (colisao_circulo_vs_circulo(g->jogador.pos, g->jogador.tamanho * 0.8f, ep, e->tamanho))
        {
            g->jogador.vida = matematica_limite_min(g->jogador.vida -= e->dano * dt, 0);
            tookHit = 1;
            particulas_criar(g, g->jogador.pos, 1, (Color){1.0f, 0.2f, 0.2f, 0.8f});
        }

        e->tempoRecargaDisparo -= dt;
        if (e->tempoRecargaDisparo <= 0.0f)
        {
            Vetor2D dirToPlayer = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(g->jogador.pos, ep));
            float enemyShotSpeed;
            float enemyShotDamage;

            if (e->tipo == INIMIGO_DIAMANTE)
            {
                if (e->contadorRajada > 0)
                {
                    float ppOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 0.5f;
                    projeteis_criar(g, ep, dirToPlayer, 0, 200.0f + g->onda * 10.0f, 25.0f + g->onda * 4.0f, 6.0f, 4.0f, ORIENTACAO_PONTO_PONTO, -1, ppOverload);
                    e->contadorRajada--;
                    e->tempoRecargaDisparo = 0.5f;
                    if (e->contadorRajada == 0)
                    {
                        e->tempoRecargaDisparo = matematica_float_aleatorio(3.0f, 7.0f);
                        e->contadorRajada = 2;
                    }
                }
                else
                {
                    e->tempoRecargaDisparo = matematica_float_aleatorio(3.0f, 7.0f);
                    e->contadorRajada = 2;
                }
            }
            else if (e->tipo == INIMIGO_PENTAGONO)
            {
                float apnOverload = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 1.0f;
                projeteis_criar(g, ep, dirToPlayer, 0, 300.0f + g->onda * 15.0f, 30.0f + g->onda * 5.0f, 6.0f, 6.0f, ORIENTACAO_ANGULO_PONTO, -1, apnOverload);
                e->tempoRecargaDisparo = matematica_float_aleatorio(2.5f, 4.0f);
            }
            else if (e->ehChefao)
            {
                Vetor2D sideA = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f - dirToPlayer.y * 0.38f, dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                Vetor2D sideB = matematica_vetor2d_normalizar(matematica_vetor2d(dirToPlayer.x * 0.92f + dirToPlayer.y * 0.38f, -dirToPlayer.x * 0.38f + dirToPlayer.y * 0.92f));
                enemyShotSpeed = 260.0f + g->onda * 18.0f;
                enemyShotDamage = 9.5f + g->onda * 1.4f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 8.0f, 4.4f, ORIENTACAO_NENHUMA, -1, 0.0f);
                projeteis_criar(g, ep, sideA, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, ORIENTACAO_NENHUMA, -1, 0.0f);
                projeteis_criar(g, ep, sideB, 0, enemyShotSpeed * 0.9f, enemyShotDamage * 0.85f, 7.0f, 4.0f, ORIENTACAO_NENHUMA, -1, 0.0f);
                e->tempoRecargaDisparo = matematica_float_aleatorio(0.8f, 1.8f) - g->onda * 0.05f;
            }
            else if (e->tipo == INIMIGO_ATIRADOR)
            {
                enemyShotSpeed = 330.0f + g->onda * 22.0f;
                enemyShotDamage = 9.0f + g->onda * 1.6f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 5.5f, 3.6f, ORIENTACAO_NENHUMA, -1, 0.0f);
                e->tempoRecargaDisparo = matematica_float_aleatorio(1.0f, 2.1f) - g->onda * 0.04f;
            }
            else if (e->tipo == INIMIGO_TANQUE)
            {
                enemyShotSpeed = 180.0f + g->onda * 14.0f;
                enemyShotDamage = 11.0f + g->onda * 1.8f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 9.0f, 4.8f, ORIENTACAO_NENHUMA, -1, 0.0f);
                e->tempoRecargaDisparo = matematica_float_aleatorio(1.8f, 3.2f) - g->onda * 0.03f;
            }
            else
            {
                enemyShotSpeed = 220.0f + g->onda * 18.0f;
                enemyShotDamage = 7.0f + g->onda * 1.4f;
                projeteis_criar(g, ep, dirToPlayer, 0, enemyShotSpeed, enemyShotDamage, 7.0f, 4.0f, ORIENTACAO_NENHUMA, -1, 0.0f);
                e->tempoRecargaDisparo = matematica_float_aleatorio(1.1f, 2.6f) - g->onda * 0.05f;
            }

            if (e->tempoRecargaDisparo < 0.45f)
            {
                e->tempoRecargaDisparo = 0.45f;
            }
        }

        for (j = 0; j < MAXIMO_PROJETEIS; ++j)
        {
            Projetil *p = &g->projetis[j];
            if (!p->ativo || !p->vemDoJogador)
                continue;

            if (colisao_circulo_vs_circulo(p->pos, p->raio, ep, e->tamanho))
            {
                p->ativo = 0;
                e->vida = matematica_limite_min(e->vida -= p->dano, 0);
                e->flashDano = 1.0f;
                particulas_criar(g, ep, 8, (Color){1.0f, 0.6f, 0.2f, 0.95f});
                tocar_som_impacto();

                if (e->vida <= 0.0f)
                {
                    e->ativo = 0;
                    g->inimigosRestantes--;
                    if (e->ehChefao)
                    {
                        g->pontuacao += (int)(1500.0f * scoreMul);
                        g->ouro += 6;
                    }
                    else if (e->tipo == INIMIGO_TANQUE)
                    {
                        g->pontuacao += (int)((230 + g->onda * 28) * scoreMul);
                        g->ouro += 2;
                    }
                    else if (e->tipo == INIMIGO_ATIRADOR)
                    {
                        g->pontuacao += (int)((150 + g->onda * 24) * scoreMul);
                        g->ouro += 1;
                    }
                    else
                    {
                        g->pontuacao += (int)((120 + g->onda * 20) * scoreMul);
                        g->ouro += 1;
                    }
                    particulas_criar(g, ep, 18, (Color){1.0f, 0.85f, 0.2f, 0.95f});
                }
                break;
            }
        }
    }

    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &g->projetis[i];
        if (!p->ativo || p->vemDoJogador)
        {
            continue;
        }

        if (colisao_circulo_vs_circulo(p->pos, p->raio, g->jogador.pos, g->jogador.tamanho * 0.75f))
        {
            p->ativo = 0;
            g->jogador.vida = matematica_limite_min(g->jogador.vida -= p->dano, 0);
            tookHit = 1;
            particulas_criar(g, g->jogador.pos, 7, (Color){1.0f, 0.3f, 0.2f, 0.9f});
            tocar_som_impacto();
        }
    }

    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particula *pt = &g->particulas[i];
        if (!pt->ativo)
            continue;

        pt->vida -= dt;
        pt->pos = matematica_vetor2d_adicao(pt->pos, matematica_vetor2d_multiplicacao(pt->vel, dt));
        pt->vel = matematica_vetor2d_multiplicacao(pt->vel, 0.96f);
        if (pt->vida <= 0.0f)
        {
            pt->ativo = 0;
        }
    }

    g->tempoDecorrido += dt;
    g->tempoRestante -= dt;
    g->pontuacao += (int)(dt * 14.0f * scoreMul);
    g->flashDano -= dt * 2.4f;
    if (g->flashDano < 0.0f)
    {
        g->flashDano = 0.0f;
    }
    if (tookHit)
    {
        g->flashDano = 0.9f;
    }

    if (g->jogador.vida <= 0.0f || g->tempoRestante <= 0.0f)
    {
        interface_encerrar_partida(g, TELA_DERROTA);
        return;
    }

    if (g->inimigosRestantes <= 0)
    {
        if (g->onda >= g->ondasParaVencer)
        {
            interface_encerrar_partida(g, TELA_VITORIA);
        }
        else
        {
            g->tempoRestante += 6.0f;
            g->pontuacao += (int)((180 + g->onda * 40) * scoreMul);
            melhorias_rolar_opcoes(g);
            g->tela = TELA_MELHORIA;
        }
    }
}

void jogo_atualizar(Jogo *g, float dt)
{
    if (!g->executando)
    {
        return;
    }

    g->tempoDelta = dt;

    g->flashMelhoria -= dt * 1.7f;
    if (g->flashMelhoria < 0.0f)
    {
        g->flashMelhoria = 0.0f;
    }
    g->temporizadorUltimaMelhoria -= dt;
    if (g->temporizadorUltimaMelhoria < 0.0f)
    {
        g->temporizadorUltimaMelhoria = 0.0f;
    }
    g->temporizadorToast -= dt;
    if (g->temporizadorToast < 0.0f)
    {
        g->temporizadorToast = 0.0f;
    }

    if (g->tela == TELA_JOGANDO)
    {
        jogo_atualizar_jogando(g, dt);
    }
}

void jogo_tecla_pressionada(Jogo *g, unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (key < 256)
    {
        g->entrada.teclas[key] = 1;
        g->entrada.teclasPressionadas[key] = 1;
    }

    if (key == 27)
    {
        if (g->tela == TELA_JOGANDO)
        {
            g->tela = TELA_PAUSADA;
        }
        else if (g->tela == TELA_PAUSADA)
        {
            g->tela = TELA_JOGANDO;
        }
        else if (g->tela == TELA_OPCOES)
        {
            g->tela = TELA_MENU;
        }
        else if (g->tela == TELA_PONTUACOES)
        {
            g->tela = TELA_MENU;
        }
    }

    if ((key == 'p' || key == 'P') && g->tela == TELA_JOGANDO)
    {
        g->tela = TELA_PAUSADA;
    }
    else if ((key == 'p' || key == 'P') && g->tela == TELA_PAUSADA)
    {
        g->tela = TELA_JOGANDO;
    }

    if (g->tela == TELA_MENU && (key == 13 || key == ' '))
    {
        jogo_reiniciar(g);
    }

    if (g->tela == TELA_MENU && (key == 'o' || key == 'O'))
    {
        g->tela = TELA_OPCOES;
    }

    if (g->tela == TELA_MENU && (key == 'l' || key == 'L'))
    {
        g->tela = TELA_PONTUACOES;
    }

    if (g->tela == TELA_PAUSADA && key == 13)
    {
        g->tela = TELA_JOGANDO;
    }

    if (g->tela == TELA_MELHORIA)
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
            if (g->ouro >= 3)
            {
                g->ouro -= 3;
                melhorias_rolar_opcoes(g);
                interface_notificar(g, "Upgrade rerolled (-3 gold)");
            }
            else
            {
                interface_notificar(g, "Not enough gold for reroll");
            }
        }
    }

    if (g->tela == TELA_OPCOES)
    {
        if (key == 'a' || key == 'A')
        {
            g->audioHabilitado = !g->audioHabilitado;
            definir_audio_ativado(g->audioHabilitado);
            persistencia_salvar_configuracoes(g->audioHabilitado, g->dificuldade);
            interface_notificar(g, g->audioHabilitado ? "Audio ON" : "Audio OFF");
        }
        else if (key == 'd' || key == 'D')
        {
            g->dificuldade = (g->dificuldade + 1) % 3;
            persistencia_salvar_configuracoes(g->audioHabilitado, g->dificuldade);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(g->dificuldade));
                interface_notificar(g, msg);
            }
        }
        else if (key == 13 || key == 'm' || key == 'M')
        {
            g->tela = TELA_MENU;
        }
    }

    if (g->tela == TELA_PONTUACOES)
    {
        int pageCount = (g->contadorTodasPontuacoes + g->tamanhoPaginaPontuacao - 1) / g->tamanhoPaginaPontuacao;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        if (key == 'a' || key == 'A')
        {
            g->paginaPontuacao--;
            if (g->paginaPontuacao < 0)
            {
                g->paginaPontuacao = 0;
            }
        }
        else if (key == 'd' || key == 'D')
        {
            g->paginaPontuacao++;
            if (g->paginaPontuacao > pageCount - 1)
            {
                g->paginaPontuacao = pageCount - 1;
            }
        }
        else if (key == 'c' || key == 'C')
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            g->paginaPontuacao = 0;
            interface_notificar(g, "Score history cleared");
        }
        else if (key == 'm' || key == 'M' || key == 13)
        {
            g->tela = TELA_MENU;
        }
    }

    if ((g->tela == TELA_VITORIA || g->tela == TELA_DERROTA) && g->inserindoNome)
    {
        int len = (int)strlen(g->nomeJogador);

        if (key == 8 && len > 0)
        {
            g->nomeJogador[len - 1] = '\0';
            return;
        }

        if (key == 13 && !g->nomeSalvo)
        {
            if (strlen(g->nomeJogador) == 0)
            {
                snprintf(g->nomeJogador, sizeof(g->nomeJogador), "Player");
            }
            persistencia_apor_pontuacao(g->nomeJogador, g->pontuacao, g->onda);
            g->nomeSalvo = 1;
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            interface_notificar(g, "Score saved");
            return;
        }

        if ((isalnum(key) || key == ' ' || key == '_') && len < (int)sizeof(g->nomeJogador) - 1)
        {
            g->nomeJogador[len] = (char)key;
            g->nomeJogador[len + 1] = '\0';
        }

        if (g->nomeSalvo && (key == 'm' || key == 'M' || key == 13))
        {
            g->tela = TELA_MENU;
            g->inserindoNome = 0;
        }
    }
}

void jogo_tecla_levantada(Jogo *g, unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->entrada.teclas[key] = 0;
    }
}

void jogo_especial_pressionado(Jogo *g, int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->entrada.especiais[key] = 1;
    }

    if (key == GLUT_KEY_F12)
    {
        time_t t = time(NULL);
        struct tm *tmv = localtime(&t);
        char filename[128];

        snprintf(filename, sizeof(filename), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.ppm",
                 tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
        if (imagem_salvar_ppm(filename, g->largura, g->altura))
        {
            interface_notificar(g, "Screenshot saved");
        }
        else
        {
            interface_notificar(g, "Screenshot failed");
        }
    }
}

void jogo_especial_levantado(Jogo *g, int key, int x, int y)
{
    (void)x;
    (void)y;
    if (key < 256)
    {
        g->entrada.especiais[key] = 0;
    }
}

void jogo_mouse_pressionado(Jogo *g, int button, int state, int x, int y)
{
    float wx;
    float wy;
    float bx;
    float by;

    g->entrada.mouseX = x;
    g->entrada.mouseY = y;

    if (button >= 0 && button < 3)
    {
        if (state == GLUT_DOWN)
        {
            g->entrada.mouseApertado[button] = 1;
            g->entrada.mousePressionado[button] = 1;
        }
        else
        {
            g->entrada.mouseApertado[button] = 0;
        }
    }

    wx = (float)x;
    wy = (float)y;

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    {
        return;
    }

    if (g->tela == TELA_MENU)
    {
        float uiScale = (float)g->largura / 1280.0f;
        float hScale = (float)g->altura / 720.0f;
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
        bx = g->largura * 0.5f - btnW * 0.5f;
        by = g->altura * 0.30f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + startH)
        {
            jogo_reiniciar(g);
            return;
        }

        by = g->altura * 0.40f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            g->tela = TELA_OPCOES;
            return;
        }

        by = g->altura * 0.50f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            g->tela = TELA_PONTUACOES;
            return;
        }
    }

    if (g->tela == TELA_OPCOES)
    {
        bx = g->largura * 0.5f - 180.0f;
        by = g->altura * 0.56f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            g->audioHabilitado = !g->audioHabilitado;
            definir_audio_ativado(g->audioHabilitado);
            persistencia_salvar_configuracoes(g->audioHabilitado, g->dificuldade);
            interface_notificar(g, g->audioHabilitado ? "Audio ON" : "Audio OFF");
            return;
        }

        by = g->altura * 0.46f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            g->dificuldade = (g->dificuldade + 1) % 3;
            persistencia_salvar_configuracoes(g->audioHabilitado, g->dificuldade);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(g->dificuldade));
                interface_notificar(g, msg);
            }
            return;
        }

        bx = g->largura * 0.5f - 110.0f;
        by = g->altura * 0.31f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            g->tela = TELA_MENU;
            return;
        }
    }

    if (g->tela == TELA_PONTUACOES)
    {
        int pageCount = (g->contadorTodasPontuacoes + g->tamanhoPaginaPontuacao - 1) / g->tamanhoPaginaPontuacao;
        if (pageCount <= 0)
        {
            pageCount = 1;
        }

        bx = g->largura * 0.5f - 255.0f;
        by = g->altura * 0.20f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            g->paginaPontuacao--;
            if (g->paginaPontuacao < 0)
            {
                g->paginaPontuacao = 0;
            }
            return;
        }

        bx = g->largura * 0.5f + 95.0f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            g->paginaPontuacao++;
            if (g->paginaPontuacao > pageCount - 1)
            {
                g->paginaPontuacao = pageCount - 1;
            }
            return;
        }

        bx = g->largura * 0.5f - 110.0f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(g);
            interface_refrescar_pontuacoes(g);
            g->paginaPontuacao = 0;
            interface_notificar(g, "Score history cleared");
            return;
        }

        by = g->altura * 0.11f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            g->tela = TELA_MENU;
            return;
        }
    }

    if (g->tela == TELA_MELHORIA)
    {
        float uiScale = (float)g->largura / 1280.0f;
        float hScale = (float)g->altura / 720.0f;
        float centerX = g->largura * 0.5f;
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
        cardY = g->altura * 0.36f;

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
        if (wx >= bx && wx <= bx + rerollW && wy >= by && wy <= by + rerollH && g->ouro >= 3)
        {
            g->ouro -= 3;
            melhorias_rolar_opcoes(g);
            interface_notificar(g, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((g->tela == TELA_VITORIA || g->tela == TELA_DERROTA) && g->nomeSalvo)
    {
        bx = g->largura * 0.5f - 110.0f;
        by = g->altura * 0.45f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            g->tela = TELA_MENU;
            g->inserindoNome = 0;
        }
    }
}

void jogo_mouse_movido(Jogo *g, int x, int y)
{
    g->entrada.mouseX = x;
    g->entrada.mouseY = y;
}

void jogo_renderizar(Jogo *g)
{
    int i;

    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderizar_perspectiva_ortografica(g);
    renderizar_fundo(g);

    if (g->tela == TELA_MENU)
        desenhar_menu(g);

    if (g->tela == TELA_PONTUACOES)
        desenhar_placar(g);

    if (g->tela == TELA_OPCOES)
        desenhar_opcoes(g);

    if (g->tela == TELA_JOGANDO || g->tela == TELA_PAUSADA || g->tela == TELA_MELHORIA || g->tela == TELA_VITORIA || g->tela == TELA_DERROTA)
    {
        renderizar_plataformas(g);
        for (i = 0; i < MAXIMO_INIMIGOS; ++i)
        {
            if (g->inimigos[i].ativo)
                renderizar_inimigo(&g->inimigos[i]);
        }
        renderizar_projeteis(g);
        renderizar_particulas(g);
        renderizar_jogador(g);
        desenhar_hud(g);
        desenhar_vida_boss(g);
        if (g->tela == TELA_JOGANDO)
            desenhar_mira(g);
    }

    if (g->tela == TELA_PAUSADA)
        desenhar_pausa(g);

    if (g->tela == TELA_MELHORIA)
        desenhar_melhorias_tela(g);

    if (g->tela == TELA_VITORIA || g->tela == TELA_DERROTA)
        desenhar_fim(g);

    desenhar_flash(g);

    glutSwapBuffers();
}

void jogo_iniciar_frame(Jogo *g)
{
    memset(g->entrada.teclasPressionadas, 0, sizeof(g->entrada.teclasPressionadas));
    memset(g->entrada.mousePressionado, 0, sizeof(g->entrada.mousePressionado));
}