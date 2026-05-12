#include "melhorias.h"
#include "cenario.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

void melhorias_preencher_opcoes(OpcaoMelhoria *o, TipoMelhoria type)
{
    o->tipo = type;
    switch (type)
    {
    case MELHORIA_DANO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Dano do Disparo");
        snprintf(o->descricao, sizeof(o->descricao), "Aumenta o dano do disparo");
        break;
    case MELHORIA_TAXA_DE_DISPARO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Taxa de Disparo");
        snprintf(o->descricao, sizeof(o->descricao), "Aumenta a taxa de disparo");
        break;
    case MELHORIA_VELOCIDADE:
        snprintf(o->rotulo, sizeof(o->rotulo), "Velocidade");
        snprintf(o->descricao, sizeof(o->descricao), "Aumenta a velocidade de movimento");
        break;
    case MELHORIA_CURAR:
        snprintf(o->rotulo, sizeof(o->rotulo), "Aumento de Vida");
        snprintf(o->descricao, sizeof(o->descricao), "Recupera e adiciona mais 50 de vida");
        break;
    case MELHORIA_TEMPO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Aumento de Tempo");
        snprintf(o->descricao, sizeof(o->descricao), "Acrescenta 20 segundos ao timer");
        break;
    case MELHORIA_ORIENTACAO_PONTO_PONTO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Balas Guiadas");
        snprintf(o->descricao, sizeof(o->descricao), "Desbloqueia projeteis guiados com 5 balas guiadas");
        break;
    case MELHORIA_ORIENTACAO_ANGULO_PONTO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Balas Guiadas+");
        snprintf(o->descricao, sizeof(o->descricao), "Melhora os projeteis guiados e adiciona mais 5 balas guiadas");
        break;
    case MELHORIA_MUNICAO:
        snprintf(o->rotulo, sizeof(o->rotulo), "Aumento de Municao");
        snprintf(o->descricao, sizeof(o->descricao), "Adiciona mais 10 balas guiadas");
        break;
    default:
        snprintf(o->rotulo, sizeof(o->rotulo), "Desconhecido");
        snprintf(o->descricao, sizeof(o->descricao), "Efeito desconhecido");
        break;
    }
}

void melhorias_rolar_opcoes(Jogo *g)
{
    int i;
    int used[MELHORIA_CONTADOR] = {0};

    if (g->jogador.possuiPontoPonto)
        used[MELHORIA_ORIENTACAO_PONTO_PONTO] = 1;
    if (g->jogador.possuiAnguloPonto)
        used[MELHORIA_ORIENTACAO_ANGULO_PONTO] = 1;

    for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
    {
        TipoMelhoria t;
        int guard = 0;
        do
        {
            t = (TipoMelhoria)(rand() % MELHORIA_CONTADOR);
            if (t == MELHORIA_ORIENTACAO_ANGULO_PONTO && !g->jogador.possuiPontoPonto)
                t = MELHORIA_DANO;
            if ((t == MELHORIA_MUNICAO) && (!g->jogador.possuiPontoPonto && !g->jogador.possuiAnguloPonto))
            {
                t = MELHORIA_DANO;
            }
            guard++;
        } while (used[t] && guard < 32);
        used[t] = 1;
        melhorias_preencher_opcoes(&g->melhorias[i], t);
    }
}

void melhorias_aplicar(Jogo *g, TipoMelhoria t)
{
    switch (t)
    {
    case MELHORIA_DANO:
        g->jogador.dano += 7.5f;
        break;
    case MELHORIA_TAXA_DE_DISPARO:
        g->jogador.taxaDeDisparo *= 0.8f;
        if (g->jogador.taxaDeDisparo < 0.1f)
        {
            g->jogador.taxaDeDisparo = 0.1f;
        }
        break;
    case MELHORIA_VELOCIDADE:
        g->jogador.velocidade += 20.0f;
        break;
    case MELHORIA_CURAR:
        g->jogador.vidaMaxima += 50.0f;
        g->jogador.vida = g->jogador.vidaMaxima;
        break;
    case MELHORIA_TEMPO:
        g->tempoRestante += 20.0f;
        break;
    case MELHORIA_ORIENTACAO_PONTO_PONTO:
        g->jogador.possuiPontoPonto = 1;
        g->jogador.municaoGuiadaMaxima += 5;
        g->jogador.municaoGuiada = g->jogador.municaoGuiadaMaxima;
        if (g->jogador.aceleracaoLateralMaxima < 100.0f)
            g->jogador.aceleracaoLateralMaxima = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_ORIENTACAO_ANGULO_PONTO:
        if (!g->jogador.possuiPontoPonto)
        {
            g->jogador.possuiPontoPonto = 1;
            g->jogador.municaoGuiadaMaxima += 10;
        }
        g->jogador.possuiAnguloPonto = 1;
        g->jogador.municaoGuiadaMaxima += 5;
        g->jogador.municaoGuiada = g->jogador.municaoGuiadaMaxima;
        if (g->jogador.aceleracaoLateralMaxima < 100.0f)
            g->jogador.aceleracaoLateralMaxima = JOGADOR_MAXIMO_ACELERACAO_LATERAL;
        break;
    case MELHORIA_MUNICAO:
        g->jogador.municaoGuiadaMaxima += 10;
        g->jogador.municaoGuiada = g->jogador.municaoGuiadaMaxima;
        break;
    default:
        break;
    }
}

void melhorias_escolher(Jogo *g, int idx)
{
    if (idx < 0 || idx >= MAXIMO_OPCOES_UPGRADE)
    {
        return;
    }

    melhorias_aplicar(g, g->melhorias[idx].tipo);
    snprintf(g->ultimaMelhoria, sizeof(g->ultimaMelhoria), "%s", g->melhorias[idx].rotulo);
    g->temporizadorUltimaMelhoria = 2.5f;
    g->flashMelhoria = 1.0f;
    tocar_som_tiro();

    g->jogador.municaoGuiada = g->jogador.municaoGuiadaMaxima;

    g->onda += 1;
    criar_onda(g);
    g->tela = TELA_JOGANDO;
}