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
    int indice_obstaculo;
    float movimento_x = 0.0f;

    if (jogo->entrada.teclas['a'] || jogo->entrada.teclas['A'] || jogo->entrada.especiais[GLUT_KEY_LEFT])
        movimento_x -= 1.0f;
    if (jogo->entrada.teclas['d'] || jogo->entrada.teclas['D'] || jogo->entrada.especiais[GLUT_KEY_RIGHT])
        movimento_x += 1.0f;

    jogo->jogador.pos.x += movimento_x * jogo->jogador.velocidade * delta_tempo;

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
        static Vetor2D posicao_anterior_jogador = {0, 0};
        static Vetor2D velocidade_anterior_jogador = {0, 0};
        if (posicao_anterior_jogador.x == 0 && posicao_anterior_jogador.y == 0)
            posicao_anterior_jogador = jogo->jogador.pos;
        jogo->jogador.vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->jogador.pos, posicao_anterior_jogador), 1.0f / delta_tempo);
        jogo->jogador.aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(jogo->jogador.vel, velocidade_anterior_jogador), 1.0f / delta_tempo);
        posicao_anterior_jogador = jogo->jogador.pos;
        velocidade_anterior_jogador = jogo->jogador.vel;
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

    for (indice_obstaculo = 0; indice_obstaculo < MAXIMO_PLATAFORMAS; ++indice_obstaculo)
    {
        Obstaculo *obstaculo = &jogo->obstaculos[indice_obstaculo];
        if (!obstaculo->ativo)
            continue;
        if (colisao_circulo_vs_retangulo(jogo->jogador.pos, jogo->jogador.tamanho,
                                         matematica_vetor2d(obstaculo->x, obstaculo->y),
                                         matematica_vetor2d(obstaculo->x + obstaculo->largura, obstaculo->y + obstaculo->altura)))
        {
            float centro_x = obstaculo->x + obstaculo->largura * 0.5f;
            float centro_y = obstaculo->y + obstaculo->altura * 0.5f;
            float diferenca_x = fabsf(jogo->jogador.pos.x - centro_x);
            float diferenca_y = fabsf(jogo->jogador.pos.y - centro_y);

            if (jogo->jogador.pos.y - jogo->jogador.tamanho < centro_y && jogo->jogador.velocidade_y >= 0.0f)
            {
                if (jogo->jogador.velocidade_y > 50.0f)
                    audio_tocar_som_pulo_fim();
                jogo->jogador.pos.y = obstaculo->y - jogo->jogador.tamanho;
                jogo->jogador.velocidade_y = 0.0f;
                jogo->jogador.esta_no_chao = 1;
            }
            else if (jogo->jogador.pos.y - jogo->jogador.tamanho >= centro_y && diferenca_x > diferenca_y)
            {
                if (jogo->jogador.pos.x < centro_x)
                    jogo->jogador.pos.x = obstaculo->x - jogo->jogador.tamanho - 5.0f;
                else
                    jogo->jogador.pos.x = obstaculo->x + obstaculo->largura + jogo->jogador.tamanho + 5.0f;
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
    int indice_inimigo;
    jogo->jogador.tempo_recarga_disparo -= delta_tempo;

    if (jogo->entrada.mouse_segurado[0] && jogo->jogador.tempo_recarga_disparo <= 0.0f)
    {
        Vetor2D posicao_alvo = matematica_mouse_para_mundo(jogo);
        Vetor2D direcao = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(posicao_alvo, jogo->jogador.pos));
        projeteis_criar(jogo, jogo->jogador.pos, direcao, 1, jogo->jogador.velocidade_projetil, jogo->jogador.dano, 6.0f, 2.5f, GUIANCA_NENHUMA, -1, 0.0f);
        particulas_criar(jogo, jogo->jogador.pos, 4, (Cor){0.3f, 0.9f, 1.0f, 0.85f});
        jogo->jogador.tempo_recarga_disparo = jogo->jogador.taxa_disparo;
        audio_tocar_som_tiro_disparo();
    }

    if (jogo->entrada.mouse_segurado[2] && jogo->jogador.tempo_recarga_disparo <= 0.0f && jogo->jogador.municao_guiada > 0)
    {
        if (jogo->jogador.tem_guianca_pp || jogo->jogador.tem_guianca_apn)
        {
            Vetor2D posicao_alvo = matematica_mouse_para_mundo(jogo);
            int melhor_alvo = -1;
            float distancia_minima = 1000.0f;
            for (indice_inimigo = 0; indice_inimigo < MAXIMO_INIMIGOS; ++indice_inimigo)
            {
                if (jogo->inimigos[indice_inimigo].ativo)
                {
                    float distancia_ao_alvo = matematica_vetor2d_len(matematica_vetor2d_subtracao(inimigo_posicao(&jogo->inimigos[indice_inimigo]), posicao_alvo));
                    if (distancia_ao_alvo < distancia_minima)
                    {
                        distancia_minima = distancia_ao_alvo;
                        melhor_alvo = indice_inimigo;
                    }
                }
            }
            if (melhor_alvo != -1)
            {
                Vetor2D direcao = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(posicao_alvo, jogo->jogador.pos));
                TipoGuianca lei = jogo->jogador.tem_guianca_apn ? GUIANCA_APN : GUIANCA_PP;
                projeteis_criar(jogo, jogo->jogador.pos, direcao, 1, jogo->jogador.velocidade * 1.3f, jogo->jogador.dano * 3.0f, 8.0f, 5.0f, lei, melhor_alvo, jogo->jogador.aceleracao_lateral_max);
                jogo->jogador.municao_guiada--;
                jogo->jogador.tempo_recarga_disparo = jogo->jogador.taxa_disparo * 2.0f;
                audio_tocar_som_tiro_disparo();
            }
        }
    }
}

static void gameplay_atualizar_projeteis_internos(Jogo *jogo, float delta_tempo)
{
    int indice_projetil;
    for (indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; ++indice_projetil)
    {
        Projetil *projetil = &jogo->projetis[indice_projetil];
        int indice_obstaculo;
        if (!projetil->ativo)
            continue;

        if (projetil->guianca != GUIANCA_NENHUMA && projetil->errou)
        {
            projetil->temporizador_deteccao -= delta_tempo;
            if (projetil->temporizador_deteccao <= 0.0f)
                projetil->ativo = 0;
            projetil->vida -= delta_tempo;
            projetil->pos = matematica_vetor2d_adicao(projetil->pos, matematica_vetor2d_multiplicacao(projetil->vel, delta_tempo));
            continue;
        }

        if (projetil->guianca != GUIANCA_NENHUMA)
        {
            float velocidade_atual = matematica_vetor2d_len(projetil->vel);
            float angulo_voo = atan2f(projetil->vel.y, projetil->vel.x);
            float comando_aceleracao = 0.0f;
            Vetor2D posicao_alvo, velocidade_alvo = {0, 0}, aceleracao_alvo = {0, 0};
            int alvo_valido = 0;

            if (projetil->vem_do_jogador)
            {
                if (projetil->indice_alvo != -1 && jogo->inimigos[projetil->indice_alvo].ativo)
                {
                    Inimigo *inimigo = &jogo->inimigos[projetil->indice_alvo];
                    posicao_alvo = inimigo_posicao(inimigo);
                    velocidade_alvo = inimigo->vel;
                    aceleracao_alvo = inimigo->aceleracao;
                    alvo_valido = 1;
                }
            }
            else
            {
                posicao_alvo = jogo->jogador.pos;
                velocidade_alvo = jogo->jogador.vel;
                aceleracao_alvo = jogo->jogador.aceleracao;
                alvo_valido = 1;
            }

            if (alvo_valido)
            {
                Vetor2D delta_posicao = matematica_vetor2d_subtracao(posicao_alvo, projetil->pos);
                float distancia = matematica_vetor2d_len(delta_posicao);
                float angulo_linha_visao = atan2f(delta_posicao.y, delta_posicao.x);
                float taxa_rotacao_linha_visao = (delta_posicao.x * (velocidade_alvo.y - projetil->vel.y) - delta_posicao.y * (velocidade_alvo.x - projetil->vel.x)) / (distancia * distancia);

                if (projetil->guianca == GUIANCA_APN && projetil->distancia_anterior < 9999.0f &&
                    distancia > projetil->distancia_anterior && distancia < 250.0f && projetil->distancia_anterior < 270.0f)
                {
                    projetil->errou = 1;
                    projetil->temporizador_deteccao = 0.2f;
                }

                if (projetil->guianca == GUIANCA_PP && !projetil->errou && projetil->distancia_anterior < 9999.0f)
                {
                    if (fabs(taxa_rotacao_linha_visao) * velocidade_atual > projetil->aceleracao_lateral_max && distancia < 300.0f)
                    {
                        projetil->errou = 1;
                        projetil->temporizador_deteccao = 0.2f;
                    }
                }

                projetil->distancia_anterior = distancia;

                if (!projetil->errou)
                {
                    if (projetil->guianca == GUIANCA_APN)
                    {
                        Vetor2D velocidade_relativa = matematica_vetor2d_subtracao(velocidade_alvo, projetil->vel);
                        float velocidade_fechamento = -(delta_posicao.x * velocidade_relativa.x + delta_posicao.y * velocidade_relativa.y) / distancia;
                        float aceleracao_rotacao_linha_visao = (delta_posicao.x * velocidade_relativa.y - delta_posicao.y * velocidade_relativa.x) / (distancia * distancia);
                        float aceleracao_alvo_perpendicular = -aceleracao_alvo.x * sinf(angulo_linha_visao) + aceleracao_alvo.y * cosf(angulo_linha_visao);
                        float ganho_navegacao = MISSIL_GUIANCA_APN_GANHO;
                        comando_aceleracao = ganho_navegacao * velocidade_fechamento * aceleracao_rotacao_linha_visao + (ganho_navegacao * 0.5f) * aceleracao_alvo_perpendicular;
                    }
                    else if (projetil->guianca == GUIANCA_PP)
                    {
                        float erro_angulo = angulo_linha_visao - angulo_voo;
                        while (erro_angulo > (float)M_PI)
                            erro_angulo -= 2.0f * (float)M_PI;
                        while (erro_angulo < -(float)M_PI)
                            erro_angulo += 2.0f * (float)M_PI;
                        comando_aceleracao = MISSIL_GUIANCA_PP_GANHO * velocidade_atual * erro_angulo;
                    }

                    if (comando_aceleracao > projetil->aceleracao_lateral_max)
                        comando_aceleracao = projetil->aceleracao_lateral_max;
                    if (comando_aceleracao < -projetil->aceleracao_lateral_max)
                        comando_aceleracao = -projetil->aceleracao_lateral_max;

                    projetil->aceleracao_lateral += (comando_aceleracao - projetil->aceleracao_lateral) * (delta_tempo / MISSIL_GUIANCA_LAG);
                }
            }

            if (!projetil->errou)
            {
                float aceleracao_lateral_x = -projetil->aceleracao_lateral * sinf(angulo_voo);
                float aceleracao_lateral_y = projetil->aceleracao_lateral * cosf(angulo_voo);
                projetil->vel.x += aceleracao_lateral_x * delta_tempo;
                projetil->vel.y += aceleracao_lateral_y * delta_tempo;

                float nova_velocidade = matematica_vetor2d_len(projetil->vel);
                if (nova_velocidade > 0.001f)
                    projetil->vel = matematica_vetor2d_multiplicacao(projetil->vel, velocidade_atual / nova_velocidade);
            }
        }

        projetil->vida -= delta_tempo;
        projetil->pos = matematica_vetor2d_adicao(projetil->pos, matematica_vetor2d_multiplicacao(projetil->vel, delta_tempo));

        if (projetil->vida <= 0.0f || projetil->pos.x < -20.0f || projetil->pos.x > jogo->largura + 20.0f ||
            projetil->pos.y < -20.0f || projetil->pos.y > jogo->altura + 20.0f)
        {
            projetil->ativo = 0;
        }

        for (indice_obstaculo = 0; indice_obstaculo < MAXIMO_PLATAFORMAS && projetil->ativo; ++indice_obstaculo)
        {
            Obstaculo *obstaculo = &jogo->obstaculos[indice_obstaculo];
            if (!obstaculo->ativo)
                continue;
            if (colisao_circulo_vs_retangulo(projetil->pos, projetil->raio,
                                             matematica_vetor2d(obstaculo->x, obstaculo->y),
                                             matematica_vetor2d(obstaculo->x + obstaculo->largura, obstaculo->y + obstaculo->altura)))
                projetil->ativo = 0;
        }
    }
}

static int gameplay_atualizar_inimigos_internos(Jogo *jogo, float delta_tempo, float multiplicador_pontuacao)
{
    int indice_inimigo;
    int recebeu_dano = 0;

    for (indice_inimigo = 0; indice_inimigo < MAXIMO_INIMIGOS; ++indice_inimigo)
    {
        Inimigo *inimigo = &jogo->inimigos[indice_inimigo];
        int indice_projetil;
        Vetor2D posicao_inimigo;
        if (!inimigo->ativo)
            continue;

        inimigo->angulo += inimigo->velocidade_angular * delta_tempo;
        inimigo->flash_dano -= delta_tempo * 4.0f;
        if (inimigo->flash_dano < 0.0f)
            inimigo->flash_dano = 0.0f;

        posicao_inimigo = inimigo_posicao(inimigo);

        {
            static Vetor2D posicao_anterior_inimigos[MAXIMO_INIMIGOS] = {{0, 0}};
            static Vetor2D velocidade_anterior_inimigos[MAXIMO_INIMIGOS] = {{0, 0}};
            if (posicao_anterior_inimigos[indice_inimigo].x == 0 && posicao_anterior_inimigos[indice_inimigo].y == 0)
                posicao_anterior_inimigos[indice_inimigo] = posicao_inimigo;
            inimigo->vel = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(posicao_inimigo, posicao_anterior_inimigos[indice_inimigo]), 1.0f / delta_tempo);
            inimigo->aceleracao = matematica_vetor2d_multiplicacao(matematica_vetor2d_subtracao(inimigo->vel, velocidade_anterior_inimigos[indice_inimigo]), 1.0f / delta_tempo);
            posicao_anterior_inimigos[indice_inimigo] = posicao_inimigo;
            velocidade_anterior_inimigos[indice_inimigo] = inimigo->vel;
        }

        if (colisao_circulo_vs_circulo(jogo->jogador.pos, jogo->jogador.tamanho * 0.8f, posicao_inimigo, inimigo->tamanho))
        {
            jogo->jogador.vida = matematica_limite_min(jogo->jogador.vida -= inimigo->dano * delta_tempo, 0);
            recebeu_dano = 1;
            particulas_criar(jogo, jogo->jogador.pos, 1, (Cor){1.0f, 0.2f, 0.2f, 0.8f});
        }

        inimigo->tempo_recarga_disparo -= delta_tempo;
        if (inimigo->tempo_recarga_disparo <= 0.0f)
        {
            Vetor2D direcao_ao_jogador = matematica_vetor2d_normalizar(matematica_vetor2d_subtracao(jogo->jogador.pos, posicao_inimigo));
            float velocidade_tiro_inimigo;
            float dano_tiro_inimigo;

            if (inimigo->tipo == INIMIGO_DIAMANTE)
            {
                if (inimigo->contador_rajada > 0)
                {
                    float sobrecarga_aceleracao_pp = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 0.5f;
                    projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, 200.0f + jogo->onda * 10.0f, 25.0f + jogo->onda * 4.0f, 6.0f, 4.0f, GUIANCA_PP, -1, sobrecarga_aceleracao_pp);
                    inimigo->contador_rajada--;
                    inimigo->tempo_recarga_disparo = 0.5f;
                    if (inimigo->contador_rajada == 0)
                    {
                        inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                        inimigo->contador_rajada = 2;
                    }
                }
                else
                {
                    inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 7.0f);
                    inimigo->contador_rajada = 2;
                }
            }
            else if (inimigo->tipo == INIMIGO_PENTAGONO)
            {
                float sobrecarga_aceleracao_apn = JOGADOR_MAXIMO_ACELERACAO_LATERAL * 1.0f;
                projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, 300.0f + jogo->onda * 15.0f, 30.0f + jogo->onda * 5.0f, 6.0f, 6.0f, GUIANCA_APN, -1, sobrecarga_aceleracao_apn);
                inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(2.5f, 4.0f);
            }
            else if (inimigo->eh_chefao)
            {
                Vetor2D direcao_lateral_a = matematica_vetor2d_normalizar(matematica_vetor2d(direcao_ao_jogador.x * 0.92f - direcao_ao_jogador.y * 0.38f, direcao_ao_jogador.x * 0.38f + direcao_ao_jogador.y * 0.92f));
                Vetor2D direcao_lateral_b = matematica_vetor2d_normalizar(matematica_vetor2d(direcao_ao_jogador.x * 0.92f + direcao_ao_jogador.y * 0.38f, -direcao_ao_jogador.x * 0.38f + direcao_ao_jogador.y * 0.92f));
                velocidade_tiro_inimigo = 260.0f + jogo->onda * 18.0f;
                dano_tiro_inimigo = 9.5f + jogo->onda * 1.4f;
                projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, velocidade_tiro_inimigo, dano_tiro_inimigo, 8.0f, 4.4f, GUIANCA_NENHUMA, -1, 0.0f);
                projeteis_criar(jogo, posicao_inimigo, direcao_lateral_a, 0, velocidade_tiro_inimigo * 0.9f, dano_tiro_inimigo * 0.85f, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                projeteis_criar(jogo, posicao_inimigo, direcao_lateral_b, 0, velocidade_tiro_inimigo * 0.9f, dano_tiro_inimigo * 0.85f, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.8f, 1.8f) - jogo->onda * 0.05f;
            }
            else if (inimigo->tipo == INIMIGO_ATIRADOR)
            {
                velocidade_tiro_inimigo = 330.0f + jogo->onda * 22.0f;
                dano_tiro_inimigo = 9.0f + jogo->onda * 1.6f;
                projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, velocidade_tiro_inimigo, dano_tiro_inimigo, 5.5f, 3.6f, GUIANCA_NENHUMA, -1, 0.0f);
                inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.0f, 2.1f) - jogo->onda * 0.04f;
            }
            else if (inimigo->tipo == INIMIGO_TANQUE)
            {
                velocidade_tiro_inimigo = 180.0f + jogo->onda * 14.0f;
                dano_tiro_inimigo = 11.0f + jogo->onda * 1.8f;
                projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, velocidade_tiro_inimigo, dano_tiro_inimigo, 9.0f, 4.8f, GUIANCA_NENHUMA, -1, 0.0f);
                inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.8f, 3.2f) - jogo->onda * 0.03f;
            }
            else
            {
                velocidade_tiro_inimigo = 220.0f + jogo->onda * 18.0f;
                dano_tiro_inimigo = 7.0f + jogo->onda * 1.4f;
                projeteis_criar(jogo, posicao_inimigo, direcao_ao_jogador, 0, velocidade_tiro_inimigo, dano_tiro_inimigo, 7.0f, 4.0f, GUIANCA_NENHUMA, -1, 0.0f);
                inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.1f, 2.6f) - jogo->onda * 0.05f;
            }

            if (inimigo->tempo_recarga_disparo < 0.45f)
                inimigo->tempo_recarga_disparo = 0.45f;
        }

        for (indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; ++indice_projetil)
        {
            Projetil *projetil = &jogo->projetis[indice_projetil];
            if (!projetil->ativo || !projetil->vem_do_jogador)
                continue;

            if (colisao_circulo_vs_circulo(projetil->pos, projetil->raio, posicao_inimigo, inimigo->tamanho))
            {
                projetil->ativo = 0;
                inimigo->vida = matematica_limite_min(inimigo->vida -= projetil->dano, 0);
                inimigo->flash_dano = 1.0f;
                particulas_criar(jogo, posicao_inimigo, 8, (Cor){1.0f, 0.6f, 0.2f, 0.95f});
                audio_tocar_som_tiro_atingido();

                if (inimigo->vida <= 0.0f)
                {
                    inimigo->ativo = 0;
                    jogo->inimigos_restantes--;
                    if (inimigo->eh_chefao)
                    {
                        jogo->pontuacao += (int)(1500.0f * multiplicador_pontuacao);
                        jogo->ouro += 6;
                    }
                    else if (inimigo->tipo == INIMIGO_TANQUE)
                    {
                        jogo->pontuacao += (int)((230 + jogo->onda * 28) * multiplicador_pontuacao);
                        jogo->ouro += 2;
                    }
                    else if (inimigo->tipo == INIMIGO_ATIRADOR)
                    {
                        jogo->pontuacao += (int)((150 + jogo->onda * 24) * multiplicador_pontuacao);
                        jogo->ouro += 1;
                    }
                    else
                    {
                        jogo->pontuacao += (int)((120 + jogo->onda * 20) * multiplicador_pontuacao);
                        jogo->ouro += 1;
                    }
                    particulas_criar(jogo, posicao_inimigo, 18, (Cor){1.0f, 0.85f, 0.2f, 0.95f});
                }
                break;
            }
        }
    }
    return recebeu_dano;
}

static int gameplay_verificar_projeteis_vs_jogador(Jogo *jogo)
{
    int indice_projetil;
    int recebeu_dano = 0;
    for (indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; ++indice_projetil)
    {
        Projetil *projetil = &jogo->projetis[indice_projetil];
        if (!projetil->ativo || projetil->vem_do_jogador)
            continue;

        if (colisao_circulo_vs_circulo(projetil->pos, projetil->raio, jogo->jogador.pos, jogo->jogador.tamanho * 0.75f))
        {
            projetil->ativo = 0;
            jogo->jogador.vida = matematica_limite_min(jogo->jogador.vida -= projetil->dano, 0);
            recebeu_dano = 1;
            particulas_criar(jogo, jogo->jogador.pos, 7, (Cor){1.0f, 0.3f, 0.2f, 0.9f});
            audio_tocar_som_tiro_atingido();
        }
    }
    return recebeu_dano;
}

static void gameplay_atualizar_particulas_internos(Jogo *jogo, float delta_tempo)
{
    int indice_particula;
    for (indice_particula = 0; indice_particula < MAXIMO_PARTICULAS; ++indice_particula)
    {
        Particula *particula = &jogo->particulas[indice_particula];
        if (!particula->ativo)
            continue;
        particula->vida -= delta_tempo;
        particula->pos = matematica_vetor2d_adicao(particula->pos, matematica_vetor2d_multiplicacao(particula->vel, delta_tempo));
        particula->vel = matematica_vetor2d_multiplicacao(particula->vel, 0.96f);
        if (particula->vida <= 0.0f)
            particula->ativo = 0;
    }
}

void gameplay_atualizar_playing(Jogo *jogo, float delta_tempo)
{
    int recebeu_dano;
    float multiplicador_pontuacao = inimigo_multiplicador_pontos(jogo->dificuldade);

    gameplay_atualizar_jogador_movimento(jogo, delta_tempo);
    gameplay_atirar_jogador(jogo, delta_tempo);
    gameplay_atualizar_projeteis_internos(jogo, delta_tempo);
    recebeu_dano = gameplay_atualizar_inimigos_internos(jogo, delta_tempo, multiplicador_pontuacao);
    recebeu_dano |= gameplay_verificar_projeteis_vs_jogador(jogo);
    gameplay_atualizar_particulas_internos(jogo, delta_tempo);

    jogo->tempo_decorrido += delta_tempo;
    jogo->tempo_restante -= delta_tempo;
    jogo->pontuacao += (int)(delta_tempo * 14.0f * multiplicador_pontuacao);
    jogo->flash_dano -= delta_tempo * 2.4f;
    if (jogo->flash_dano < 0.0f)
        jogo->flash_dano = 0.0f;
    if (recebeu_dano)
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
            jogo->pontuacao += (int)((180 + jogo->onda * 40) * multiplicador_pontuacao);
            melhorias_rolar_opcoes(jogo);
            jogo->tela = TELA_MELHORIA;
        }
    }
}
