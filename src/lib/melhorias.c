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
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Recupera a vida e adiciona mais 100 ao total");
        break;
    case MELHORIA_TEMPO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Aumento de Tempo");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Acrescenta 200 segundos ao timer");
        break;
    case MELHORIA_GUIANCA_PP:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Balas Guiadas");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Desbloqueia projeteis guiados com 8 balas guiadas");
        break;
    case MELHORIA_GUIANCA_APN:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Balas Guiadas+");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Melhora os projeteis guiados e adiciona mais 8 balas guiadas");
        break;
    case MELHORIA_MUNICAO:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Aumento de Municao");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Adiciona mais 16 balas guiadas");
        break;
    default:
        snprintf(opcoes->rotulo, sizeof(opcoes->rotulo), "Desconhecido");
        snprintf(opcoes->descricao, sizeof(opcoes->descricao), "Efeito desconhecido");
        break;
    }
}

void melhorias_rolar_opcoes(Jogo *jogo)
{
    int indice_opcao;
    int tipos_usados[MELHORIA_CONTADOR] = {0};

    if (jogo->jogador.tem_guianca_pp)
        tipos_usados[MELHORIA_GUIANCA_PP] = 1;
    if (jogo->jogador.tem_guianca_apn)
        tipos_usados[MELHORIA_GUIANCA_APN] = 1;

    for (indice_opcao = 0; indice_opcao < MAXIMO_OPCOES_UPGRADE; ++indice_opcao)
    {
        TipoMelhoria tipo_sorteado;
        int contador_tentativas = 0;
        do
        {
            tipo_sorteado = (TipoMelhoria)(rand() % MELHORIA_CONTADOR);
            if (tipo_sorteado == MELHORIA_GUIANCA_APN && !jogo->jogador.tem_guianca_pp)
                tipo_sorteado = MELHORIA_DANO;
            if ((tipo_sorteado == MELHORIA_MUNICAO) && (!jogo->jogador.tem_guianca_pp && !jogo->jogador.tem_guianca_apn))
            {
                tipo_sorteado = MELHORIA_DANO;
            }
            contador_tentativas++;
        } while (tipos_usados[tipo_sorteado] && contador_tentativas < 32);
        tipos_usados[tipo_sorteado] = 1;
        melhorias_preencher_opcoes(&jogo->melhorias[indice_opcao], tipo_sorteado);
    }
}

void melhorias_aplicar(Jogo *jogo, TipoMelhoria tipo)
{
    switch (tipo)
    {
    case MELHORIA_DANO:
        jogo->jogador.dano += 10.0f;
        break;
    case MELHORIA_TAXA_DISPARO:
        jogo->jogador.taxa_disparo *= 0.6f;
        if (jogo->jogador.taxa_disparo < 0.05f)
        {
            jogo->jogador.taxa_disparo = 0.05f;
        }
        break;
    case MELHORIA_VELOCIDADE:
        jogo->jogador.velocidade += 20.0f;
        break;
    case MELHORIA_CURA:
        jogo->jogador.vida_maxima += 100.0f;
        jogo->jogador.vida = jogo->jogador.vida_maxima;
        break;
    case MELHORIA_TEMPO:
        jogo->tempo_restante += 200.0f;
        break;
    case MELHORIA_GUIANCA_PP:
        jogo->jogador.tem_guianca_pp = 1;
        jogo->jogador.municao_guiada_max += 8;
        jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;
        if (jogo->jogador.aceleracao_lateral_max < 100.0f)
            jogo->jogador.aceleracao_lateral_max = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_GUIANCA_APN:
        if (!jogo->jogador.tem_guianca_pp)
        {
            jogo->jogador.tem_guianca_pp = 1;
            jogo->jogador.municao_guiada_max += 8;
        }
        jogo->jogador.tem_guianca_apn = 1;
        jogo->jogador.municao_guiada_max += 8;
        jogo->jogador.municao_guiada = jogo->jogador.municao_guiada_max;
        if (jogo->jogador.aceleracao_lateral_max < 100.0f)
            jogo->jogador.aceleracao_lateral_max = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_MUNICAO:
        jogo->jogador.municao_guiada_max += 16;
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
