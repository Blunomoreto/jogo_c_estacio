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
            int indice_opcao = tecla - '1';
            if (indice_opcao >= 0 && indice_opcao < MAXIMO_OPCOES_UPGRADE)
                melhorias_escolher(jogo, indice_opcao);
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
                char texto_mensagem[64];
                snprintf(texto_mensagem, sizeof(texto_mensagem), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
                interface_notificar(jogo, texto_mensagem);
            }
        }
        else if (tecla == 13 || tecla == 'm' || tecla == 'M')
        {
            jogo->tela = TELA_MENU;
        }
    }

    if (jogo->tela == TELA_PONTUACOES)
    {
        int total_paginas = (jogo->numero_todas_pontuacoes + jogo->itens_por_pagina_pontuacao - 1) / jogo->itens_por_pagina_pontuacao;
        if (total_paginas <= 0)
            total_paginas = 1;

        if (tecla == 'a' || tecla == 'A')
        {
            jogo->pagina_pontuacao--;
            if (jogo->pagina_pontuacao < 0)
                jogo->pagina_pontuacao = 0;
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->pagina_pontuacao++;
            if (jogo->pagina_pontuacao > total_paginas - 1)
                jogo->pagina_pontuacao = total_paginas - 1;
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
        int comprimento_nome = (int)strlen(jogo->nome_jogador);

        if (tecla == 8 && comprimento_nome > 0)
        {
            jogo->nome_jogador[comprimento_nome - 1] = '\0';
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

        if ((isalnum(tecla) || tecla == ' ' || tecla == '_') && comprimento_nome < (int)sizeof(jogo->nome_jogador) - 1)
        {
            jogo->nome_jogador[comprimento_nome] = (char)tecla;
            jogo->nome_jogador[comprimento_nome + 1] = '\0';
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
        time_t tempo_atual = time(NULL);
        struct tm *tempo_local = localtime(&tempo_atual);
        char nome_arquivo[128];
        snprintf(nome_arquivo, sizeof(nome_arquivo), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.png",
                 tempo_local->tm_year + 1900, tempo_local->tm_mon + 1, tempo_local->tm_mday,
                 tempo_local->tm_hour, tempo_local->tm_min, tempo_local->tm_sec);
        if (imagem_salvar_png(nome_arquivo, jogo->largura, jogo->altura))
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
    float clique_x;
    float clique_y;
    float pos_x_botao;
    float pos_y_botao;

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

    clique_x = (float)x;
    clique_y = (float)y;

    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN)
        return;

    if (jogo->tela == TELA_MENU)
    {
        float escala_ui = (float)jogo->largura / 1280.0f;
        float escala_altura = (float)jogo->altura / 720.0f;
        float largura_botao;
        float altura_botao_inicio;
        float altura_botao_sub;
        if (escala_altura < escala_ui)
            escala_ui = escala_altura;
        if (escala_ui < 0.75f)
            escala_ui = 0.75f;
        if (escala_ui > 1.15f)
            escala_ui = 1.15f;

        largura_botao = 300.0f * escala_ui;
        altura_botao_inicio = 60.0f * escala_ui;
        altura_botao_sub = 50.0f * escala_ui;
        pos_x_botao = jogo->largura * 0.5f - largura_botao * 0.5f;
        pos_y_botao = jogo->altura * 0.30f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + largura_botao && clique_y >= pos_y_botao && clique_y <= pos_y_botao + altura_botao_inicio)
        {
            jogo_reiniciar(jogo);
            return;
        }

        pos_y_botao = jogo->altura * 0.40f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + largura_botao && clique_y >= pos_y_botao && clique_y <= pos_y_botao + altura_botao_sub)
        {
            jogo->tela = TELA_OPCOES;
            return;
        }

        pos_y_botao = jogo->altura * 0.50f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + largura_botao && clique_y >= pos_y_botao && clique_y <= pos_y_botao + altura_botao_sub)
        {
            jogo->tela = TELA_PONTUACOES;
            return;
        }
    }

    if (jogo->tela == TELA_OPCOES)
    {
        pos_x_botao = jogo->largura * 0.5f - 180.0f;
        pos_y_botao = jogo->altura * 0.56f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 360.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 56.0f)
        {
            jogo->audio_habilitado = !jogo->audio_habilitado;
            audio_definir_ativacao(jogo->audio_habilitado);
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            interface_notificar(jogo, jogo->audio_habilitado ? "Audio ON" : "Audio OFF");
            return;
        }

        pos_y_botao = jogo->altura * 0.46f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 360.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 56.0f)
        {
            jogo->dificuldade = (jogo->dificuldade + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audio_habilitado, jogo->dificuldade);
            {
                char texto_mensagem[64];
                snprintf(texto_mensagem, sizeof(texto_mensagem), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
                interface_notificar(jogo, texto_mensagem);
            }
            return;
        }

        pos_x_botao = jogo->largura * 0.5f - 110.0f;
        pos_y_botao = jogo->altura * 0.31f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 220.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 50.0f)
        {
            jogo->tela = TELA_MENU;
            return;
        }
    }

    if (jogo->tela == TELA_PONTUACOES)
    {
        int total_paginas = (jogo->numero_todas_pontuacoes + jogo->itens_por_pagina_pontuacao - 1) / jogo->itens_por_pagina_pontuacao;
        if (total_paginas <= 0)
            total_paginas = 1;

        pos_x_botao = jogo->largura * 0.5f - 255.0f;
        pos_y_botao = jogo->altura * 0.20f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 160.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 44.0f)
        {
            jogo->pagina_pontuacao--;
            if (jogo->pagina_pontuacao < 0)
                jogo->pagina_pontuacao = 0;
            return;
        }

        pos_x_botao = jogo->largura * 0.5f + 95.0f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 160.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 44.0f)
        {
            jogo->pagina_pontuacao++;
            if (jogo->pagina_pontuacao > total_paginas - 1)
                jogo->pagina_pontuacao = total_paginas - 1;
            return;
        }

        pos_x_botao = jogo->largura * 0.5f - 110.0f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 220.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->pagina_pontuacao = 0;
            interface_notificar(jogo, "Score history cleared");
            return;
        }

        pos_y_botao = jogo->altura * 0.11f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 220.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 44.0f)
        {
            jogo->tela = TELA_MENU;
            return;
        }
    }

    if (jogo->tela == TELA_MELHORIA)
    {
        float escala_ui = (float)jogo->largura / 1280.0f;
        float escala_altura = (float)jogo->altura / 720.0f;
        float centro_x = jogo->largura * 0.5f;
        float largura_cartao;
        float altura_cartao;
        float espacamento_cartao;
        float largura_total_cartoes;
        float pos_x_inicio_cartoes;
        float pos_y_cartoes;
        float largura_rerolar;
        float altura_rerolar;
        float pos_x_rerolar;
        float pos_y_rerolar;
        int indice_opcao;

        if (escala_altura < escala_ui)
            escala_ui = escala_altura;
        if (escala_ui < 0.78f)
            escala_ui = 0.78f;
        if (escala_ui > 1.08f)
            escala_ui = 1.08f;

        largura_cartao = 170.0f * escala_ui;
        altura_cartao = 130.0f * escala_ui;
        espacamento_cartao = 20.0f * escala_ui;
        largura_total_cartoes = largura_cartao * MAXIMO_OPCOES_UPGRADE + espacamento_cartao * (MAXIMO_OPCOES_UPGRADE - 1);
        pos_x_inicio_cartoes = centro_x - largura_total_cartoes * 0.5f;
        pos_y_cartoes = jogo->altura * 0.36f;

        largura_rerolar = 220.0f * escala_ui;
        altura_rerolar = 44.0f * escala_ui;
        pos_x_rerolar = centro_x - largura_rerolar * 0.5f;
        pos_y_rerolar = pos_y_cartoes + altura_cartao + 20.0f * escala_ui;

        for (indice_opcao = 0; indice_opcao < MAXIMO_OPCOES_UPGRADE; ++indice_opcao)
        {
            pos_x_botao = pos_x_inicio_cartoes + indice_opcao * (largura_cartao + espacamento_cartao);
            pos_y_botao = pos_y_cartoes;
            if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + largura_cartao && clique_y >= pos_y_botao && clique_y <= pos_y_botao + altura_cartao)
            {
                melhorias_escolher(jogo, indice_opcao);
                return;
            }
        }

        pos_x_botao = pos_x_rerolar;
        pos_y_botao = pos_y_rerolar;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + largura_rerolar && clique_y >= pos_y_botao && clique_y <= pos_y_botao + altura_rerolar && jogo->ouro >= 3)
        {
            jogo->ouro -= 3;
            melhorias_rolar_opcoes(jogo);
            interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((jogo->tela == TELA_VITORIA || jogo->tela == TELA_DERROTA) && jogo->nome_salvo)
    {
        pos_x_botao = jogo->largura * 0.5f - 110.0f;
        pos_y_botao = jogo->altura * 0.45f;
        if (clique_x >= pos_x_botao && clique_x <= pos_x_botao + 220.0f && clique_y >= pos_y_botao && clique_y <= pos_y_botao + 50.0f)
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
