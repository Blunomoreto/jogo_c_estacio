#include "melhorias.h"
#include "cenario.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

void melhorias_preencher_opcoes(OpcaoMelhoria *opcoes, TipoMelhoria tipo)
{
    opcoes->tipo = tipo;
    switch (tipo)
    {
    case MELHORIA_DANO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Dano do Disparo");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Aumenta o dano do disparo");
        break;
    case MELHORIA_TAXA_DISPARO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Taxa de Disparo");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Aumenta a taxa de disparo");
        break;
    case MELHORIA_VELOCIDADE:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Velocidade");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Aumenta a velocidade de movimento");
        break;
    case MELHORIA_CURA:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Aumento de Vida");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Recupera e adiciona mais 50 de vida");
        break;
    case MELHORIA_TEMPO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Aumento de Tempo");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Acrescenta 20 segundos ao timer");
        break;
    case MELHORIA_GUIANCA_PP:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Balas Guiadas");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Desbloqueia projeteis guiados com 5 balas guiadas");
        break;
    case MELHORIA_GUIANCA_APN:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Balas Guiadas+");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Melhora os projeteis guiados e adiciona mais 5 balas guiadas");
        break;
    case MELHORIA_MUNICAO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Aumento de Municao");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Adiciona mais 10 balas guiadas");
        break;
    default:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Desconhecido");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Efeito desconhecido");
        break;
    }
}

void melhorias_rolar_opcoes(Jogo *jogo)
{
    int i;
    int used[MELHORIA_CONTADOR] = {0};

    if (jogo->jogador.tem_guianca_pp)
        used[MELHORIA_GUIANCA_PP] = 1;
    if (jogo->jogador.tem_guianca_apn)
        used[MELHORIA_GUIANCA_APN] = 1;

    for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
    {
        TipoMelhoria t;
        int guard = 0;
        do
        {
            t = (TipoMelhoria)(rand() % MELHORIA_CONTADOR);
            if (t == MELHORIA_GUIANCA_APN && !jogo->jogador.tem_guianca_pp)
                t = MELHORIA_DANO;
            if ((t == MELHORIA_MUNICAO) && (!jogo->jogador.tem_guianca_pp && !jogo->jogador.tem_guianca_apn))
            {
                t = MELHORIA_DANO;
            }
            guard++;
        } while (used[t] && guard < 32);
        used[t] = 1;
        melhorias_preencher_opcoes(&jogo->melhorias[i], t);
    }
}

void melhorias_aplicar(Jogo *jogo, TipoMelhoria t)
{
    switch (t)
    {
    case MELHORIA_DANO:
        jogo->jogador.dano += 7.5f;
        break;
    case MELHORIA_TAXA_DISPARO:
        jogo->jogador.taxa_disparo *= 0.8f;
        if (jogo->jogador.taxa_disparo < 0.1f)
        {
            jogo->jogador.taxa_disparo = 0.1f;
        }
        break;
    case MELHORIA_VELOCIDADE:
        jogo->jogador.velocidade += 20.0f;
        break;
    case MELHORIA_CURA:
        jogo->jogador.vida_maxima += 50.0f;
        jogo->jogador.vida = jogo->jogador.vida_maxima;
        break;
    case MELHORIA_TEMPO:
        jogo->tempo_restante += 20.0f;
        break;
    case MELHORIA_GUIANCA_PP:
        jogo->jogador.tem_guianca_pp = 1;
        jogo->jogador.municao_guiada_max += 5;
        jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;
        if (jogo->jogador.aceleracao_lateral_max < 100.0f)
            jogo->jogador.aceleracao_lateral_max = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_GUIANCA_APN:
        if (!jogo->jogador.tem_guianca_pp)
        {
            jogo->jogador.tem_guianca_pp = 1;
            jogo->jogador.municao_guiada_max += 10;
        }
        jogo->jogador.tem_guianca_apn = 1;
        jogo->jogador.municao_guiada_max += 5;
        jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;
        if (jogo->jogador.aceleracao_lateral_max < 100.0f)
            jogo->jogador.aceleracao_lateral_max = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_MUNICAO:
        jogo->jogador.municao_guiada_max += 10;
        jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;
        break;
    default:
        break;
    }
}

void melhorias_escolher(Jogo *jogo, int identificador)
{
    if (identificador < 0 || identificador >= MAXIMO_OPCOES_UPGRADE)
    {
        return;
    }

    melhorias_aplicar(jogo, jogo->melhorias[identificador].tipo);
    snprintf(jogo->ultima_melhoria, sizeof(jogo->ultima_melhoria), "%s", jogo->melhorias[identificador].rotulo);
    jogo->temporizador_ultima_melhoria = 2.5f;
    jogo->flash_melhoria = 1.0f;
    audio_tocar_som_tiro_disparo();

    jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;

    jogo->onda += 1;
    cenario_criar_onda(jogo);
    jogo->tela = TELA_JOGANDO;
}
