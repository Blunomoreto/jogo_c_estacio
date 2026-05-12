#include "entrada.h"
#include "jogo.h"
#include "audio.h"
#include "imagem.h"
#include "inimigo.h"
#include "interface.h"
#include "melhorias.h"
#include "persistencia.h"

#include <GL/glut.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void entrada_tecla_pressionada(Jogo *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla < 256)
    {
        jogo->entrada.teclas[tecla] = 1;
        jogo->entrada.teclas_pressionadas[tecla] = 1;
    }

    if (tecla == 27)
    {
        if (jogo->tela == TELA_JOGANDO)
            jogo->tela = TELA_PAUSADA;
        else if (jogo->tela == TELA_PAUSADA)
            jogo->tela = TELA_JOGANDO;
        else if (jogo->tela == TELA_OPCOES)
            jogo->tela = TELA_MENU;
        else if (jogo->tela == TELA_PONTUACOES)
            jogo->tela = TELA_MENU;
    }

    if ((tecla == 'p' || tecla == 'P') && jogo->tela == TELA_JOGANDO)
        jogo->tela = TELA_PAUSADA;
    else if ((tecla == 'p' || tecla == 'P') && jogo->tela == TELA_PAUSADA)
        jogo->tela = TELA_JOGANDO;

    if (jogo->tela == TELA_MENU && (tecla == 13 || tecla == ' '))
        jogo_reiniciar(jogo);

    if (jogo->tela == TELA_MENU && (tecla == 'o' || tecla == 'O'))
        jogo->tela = TELA_OPCOES;

    if (jogo->tela == TELA_MENU && (tecla == 'l' || tecla == 'L'))
        jogo->tela = TELA_PONTUACOES;

    if (jogo->tela == TELA_PAUSADA && tecla == 13)
        jogo->tela = TELA_JOGANDO;

    if (jogo->tela == TELA_MELHORIA)
    {
        if (tecla == '1' || tecla == '2' || tecla == '3')
        {
            int idx = tecla - '1';
            if (idx >= 0 && idx < MAXIMO_OPCOES_UPGRADE)
                melhorias_escolher(jogo, idx);
        }
        else if (tecla == 'r' || tecla == 'R')
        {
            if (jogo->ouro >= 3)
            {
                jogo->ouro -= 3;
                melhorias_rolar_opcoes(jogo);
                interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            }
            else
            {
                interface_notificar(jogo, "Not enough gold for reroll");
            }
        }
    }

    if (jogo->tela == TELA_OPCOES)
    {
        if (tecla == 'a' || tecla == 'A')
        {
            jogo->audio_habilitado = !jogo->audio_habilitado;
            audio_definir_ativacao(jogo->audio_habilitado);
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            interface_notificar(jogo, jogo->audio_habilitado ? "Audio ON" : "Audio OFF");
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->dificuldade = (jogo->dificuldade + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
                interface_notificar(jogo, msg);
            }
        }
        else if (tecla == 13 || tecla == 'm' || tecla == 'M')
        {
            jogo->tela = TELA_MENU;
        }
    }

    if (jogo->tela == TELA_PONTUACOES)
    {
        int pageCount = (jogo->numero_todas_pontuacoes + jogo->itens_por_pagina_pontuacao - 1) / jogo->itens_por_pagina_pontuacao;
        if (pageCount <= 0)
            pageCount = 1;

        if (tecla == 'a' || tecla == 'A')
        {
            jogo->pagina_pontuacao--;
            if (jogo->pagina_pontuacao < 0)
                jogo->pagina_pontuacao = 0;
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->pagina_pontuacao++;
            if (jogo->pagina_pontuacao > pageCount - 1)
                jogo->pagina_pontuacao = pageCount - 1;
        }
        else if (tecla == 'c' || tecla == 'C')
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->pagina_pontuacao = 0;
            interface_notificar(jogo, "Score history cleared");
        }
        else if (tecla == 'm' || tecla == 'M' || tecla == 13)
        {
            jogo->tela = TELA_MENU;
        }
    }

    if ((jogo->tela == TELA_VITORIA || jogo->tela == TELA_DERROTA) && jogo->inserindo_nome)
    {
        int len = (int)strlen(jogo->nome_jogador);

        if (tecla == 8 && len > 0)
        {
            jogo->nome_jogador[len - 1] = '\0';
            return;
        }

        if (tecla == 13 && !jogo->nome_salvo)
        {
            if (strlen(jogo->nome_jogador) == 0)
                snprintf(jogo->nome_jogador, sizeof(jogo->nome_jogador), "Player");
            persistencia_apor_pontuacao(jogo->nome_jogador, jogo->pontuacao, jogo->onda);
            jogo->nome_salvo = 1;
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            interface_notificar(jogo, "Score saved");
            return;
        }

        if ((isalnum(tecla) || tecla == ' ' || tecla == '_') && len < (int)sizeof(jogo->nome_jogador) - 1)
        {
            jogo->nome_jogador[len] = (char)tecla;
            jogo->nome_jogador[len + 1] = '\0';
        }

        if (jogo->nome_salvo && (tecla == 'm' || tecla == 'M' || tecla == 13))
        {
            jogo->tela = TELA_MENU;
            jogo->inserindo_nome = 0;
        }
    }
}

void entrada_tecla_levantada(Jogo *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->entrada.teclas[tecla] = 0;
}

void entrada_especial_pressionado(Jogo *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->entrada.especiais[tecla] = 1;

    if (tecla == GLUT_KEY_F12)
    {
        time_t t = time(NULL);
        struct tm *tmv = localtime(&t);
        char filename[128];
        snprintf(filename, sizeof(filename), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.ppm",
                 tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
        if (imagem_salvar_ppm(filename, jogo->largura, jogo->altura))
            interface_notificar(jogo, "Screenshot saved");
        else
            interface_notificar(jogo, "Screenshot failed");
    }
}

void entrada_especial_levantado(Jogo *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->entrada.especiais[tecla] = 0;
}

void entrada_mouse_pressionado(Jogo *jogo, int botao, int estado, int x, int y)
{
    float wx;
    float wy;
    float bx;
    float by;

    jogo->entrada.mouse_x = x;
    jogo->entrada.mouse_y = y;

    if (botao >= 0 && botao < 3)
    {
        if (estado == GLUT_DOWN)
        {
            jogo->entrada.mouse_segurado[botao] = 1;
            jogo->entrada.mouse_clicado[botao] = 1;
        }
        else
        {
            jogo->entrada.mouse_segurado[botao] = 0;
        }
    }

    wx = (float)x;
    wy = (float)y;

    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN)
        return;

    if (jogo->tela == TELA_MENU)
    {
        float uiScale = (float)jogo->largura / 1280.0f;
        float hScale = (float)jogo->altura / 720.0f;
        float btnW;
        float startH;
        float subH;
        if (hScale < uiScale)
            uiScale = hScale;
        if (uiScale < 0.75f)
            uiScale = 0.75f;
        if (uiScale > 1.15f)
            uiScale = 1.15f;

        btnW = 300.0f * uiScale;
        startH = 60.0f * uiScale;
        subH = 50.0f * uiScale;
        bx = jogo->largura * 0.5f - btnW * 0.5f;
        by = jogo->altura * 0.30f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + startH)
        {
            jogo_reiniciar(jogo);
            return;
        }

        by = jogo->altura * 0.40f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->tela = TELA_OPCOES;
            return;
        }

        by = jogo->altura * 0.50f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->tela = TELA_PONTUACOES;
            return;
        }
    }

    if (jogo->tela == TELA_OPCOES)
    {
        bx = jogo->largura * 0.5f - 180.0f;
        by = jogo->altura * 0.56f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->audio_habilitado = !jogo->audio_habilitado;
            audio_definir_ativacao(jogo->audio_habilitado);
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            interface_notificar(jogo, jogo->audio_habilitado ? "Audio ON" : "Audio OFF");
            return;
        }

        by = jogo->altura * 0.46f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->dificuldade = (jogo->dificuldade + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
                interface_notificar(jogo, msg);
            }
            return;
        }

        bx = jogo->largura * 0.5f - 110.0f;
        by = jogo->altura * 0.31f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->tela = TELA_MENU;
            return;
        }
    }

    if (jogo->tela == TELA_PONTUACOES)
    {
        int pageCount = (jogo->numero_todas_pontuacoes + jogo->itens_por_pagina_pontuacao - 1) / jogo->itens_por_pagina_pontuacao;
        if (pageCount <= 0)
            pageCount = 1;

        bx = jogo->largura * 0.5f - 255.0f;
        by = jogo->altura * 0.20f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->pagina_pontuacao--;
            if (jogo->pagina_pontuacao < 0)
                jogo->pagina_pontuacao = 0;
            return;
        }

        bx = jogo->largura * 0.5f + 95.0f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->pagina_pontuacao++;
            if (jogo->pagina_pontuacao > pageCount - 1)
                jogo->pagina_pontuacao = pageCount - 1;
            return;
        }

        bx = jogo->largura * 0.5f - 110.0f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->pagina_pontuacao = 0;
            interface_notificar(jogo, "Score history cleared");
            return;
        }

        by = jogo->altura * 0.11f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->tela = TELA_MENU;
            return;
        }
    }

    if (jogo->tela == TELA_MELHORIA)
    {
        float uiScale = (float)jogo->largura / 1280.0f;
        float hScale = (float)jogo->altura / 720.0f;
        float centerX = jogo->largura * 0.5f;
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
            uiScale = hScale;
        if (uiScale < 0.78f)
            uiScale = 0.78f;
        if (uiScale > 1.08f)
            uiScale = 1.08f;

        cardW = 170.0f * uiScale;
        cardH = 130.0f * uiScale;
        cardGap = 20.0f * uiScale;
        totalW = cardW * MAXIMO_OPCOES_UPGRADE + cardGap * (MAXIMO_OPCOES_UPGRADE - 1);
        startX = centerX - totalW * 0.5f;
        cardY = jogo->altura * 0.36f;

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
        if (wx >= bx && wx <= bx + rerollW && wy >= by && wy <= by + rerollH && jogo->ouro >= 3)
        {
            jogo->ouro -= 3;
            melhorias_rolar_opcoes(jogo);
            interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((jogo->tela == TELA_VITORIA || jogo->tela == TELA_DERROTA) && jogo->nome_salvo)
    {
        bx = jogo->largura * 0.5f - 110.0f;
        by = jogo->altura * 0.45f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->tela = TELA_MENU;
            jogo->inserindo_nome = 0;
        }
    }
}

void entrada_mouse_movido(Jogo *jogo, int x, int y)
{
    jogo->entrada.mouse_x = x;
    jogo->entrada.mouse_y = y;
}

void entrada_iniciar_frame(Jogo *jogo)
{
    memset(jogo->entrada.teclas_pressionadas, 0, sizeof(jogo->entrada.teclas_pressionadas));
    memset(jogo->entrada.mouse_clicado, 0, sizeof(jogo->entrada.mouse_clicado));
}
