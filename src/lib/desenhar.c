#include "desenhar.h"
#include "renderizar.h"
#include "matematica.h"
#include "inimigo.h"

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

void desenhar_texto(float x, float y, const char *texto, void *fonte, float r, float g, float b)
{
    const unsigned char *p = (const unsigned char *)texto;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*p)
        glutBitmapCharacter(fonte, *p++);
}

void desenhar_texto_centralizado(float x, float y, const char *texto, void *fonte, float r, float g, float b)
{
    float largura_texto = (float)glutBitmapLength(fonte, (const unsigned char *)texto);
    desenhar_texto(x - largura_texto * 0.5f, y, texto, fonte, r, g, b);
}

void desenhar_texto_limites(float x, float y, const char *texto, void *fonte, float r, float g, float b, float largMax)
{
    const unsigned char *ponteiro_caractere = (const unsigned char *)texto;
    float pos_x_atual = x;
    float pos_y_atual = y;
    char buffer_palavra[64];
    int indice_palavra = 0;
    glColor3f(r, g, b);

    while (*ponteiro_caractere)
    {
        if (*ponteiro_caractere == ' ' || *ponteiro_caractere == '\0')
        {
            buffer_palavra[indice_palavra] = '\0';
            float largura_palavra = 0;
            for (int indice_char = 0; indice_char < indice_palavra; indice_char++)
                largura_palavra += glutBitmapWidth(fonte, buffer_palavra[indice_char]);

            if (pos_x_atual + largura_palavra > x + largMax)
            {
                pos_x_atual = x;
                pos_y_atual += 15.0f;
            }

            glRasterPos2f(pos_x_atual, pos_y_atual);
            for (int indice_char = 0; indice_char < indice_palavra; indice_char++)
                glutBitmapCharacter(fonte, buffer_palavra[indice_char]);
            pos_x_atual += largura_palavra + glutBitmapWidth(fonte, ' ');
            indice_palavra = 0;
            if (*ponteiro_caractere == '\0')
                break;
        }
        else
        {
            if (indice_palavra < 63)
                buffer_palavra[indice_palavra++] = *ponteiro_caractere;
        }
        ponteiro_caractere++;
    }
    if (indice_palavra > 0)
    {
        buffer_palavra[indice_palavra] = '\0';
        glRasterPos2f(pos_x_atual, pos_y_atual);
        for (int indice_char = 0; indice_char < indice_palavra; indice_char++)
            glutBitmapCharacter(fonte, buffer_palavra[indice_char]);
    }
}

void desenhar_hud(Jogo *jogo)
{
    char linha[128];

    renderizar_retangulo(15.0f, 52.0f, 450.0f, 62.0f, (Cor){0.0f, 0.0f, 0.0f, 0.35f});
    snprintf(linha, sizeof(linha), "HP: %.0f/%.0f   Time: %.0f   Score: %d   Wave: %d   Gold: %d",
             jogo->jogador.vida, jogo->jogador.vida_maxima, jogo->tempo_restante, jogo->pontuacao, jogo->onda, jogo->ouro);
    desenhar_texto(26.0f, 88.0f, linha, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);

    renderizar_retangulo(26.0f, 108.0f, 180.0f, 10.0f, (Cor){0.2f, 0.2f, 0.2f, 0.9f});
    renderizar_retangulo(26.0f, 108.0f, 180.0f * (jogo->jogador.vida / jogo->jogador.vida_maxima), 10.0f, (Cor){0.2f, 0.85f, 0.35f, 0.9f});

    if (jogo->jogador.tem_guianca_pp || jogo->jogador.tem_guianca_apn)
    {
        char texto_municao[32];
        snprintf(texto_municao, sizeof(texto_municao), "Missiles: %d/%d", jogo->jogador.municao_guiada, jogo->jogador.municao_guiada_max);
        desenhar_texto(220.0f, 118.0f, texto_municao, GLUT_BITMAP_HELVETICA_12, 1.0f, 0.6f, 0.2f);
    }

    if (jogo->temporizador_ultima_melhoria > 0.0f)
    {
        char texto_mensagem[128];
        snprintf(texto_mensagem, sizeof(texto_mensagem), "Upgrade: %s", jogo->ultima_melhoria);
        renderizar_retangulo(jogo->largura - 280.0f, 68.0f, 250.0f, 36.0f, (Cor){0.1f, 0.2f, 0.35f, 0.6f});
        desenhar_texto(jogo->largura - 266.0f, 90.0f, texto_mensagem, GLUT_BITMAP_HELVETICA_18, 0.9f, 0.98f, 1.0f);
    }

    if (jogo->temporizador_toast > 0.0f && jogo->mensagem_toast[0] != '\0')
    {
        renderizar_retangulo(jogo->largura - 320.0f, 26.0f, 290.0f, 34.0f, (Cor){0.06f, 0.12f, 0.25f, 0.72f});
        desenhar_texto(jogo->largura - 306.0f, 47.0f, jogo->mensagem_toast, GLUT_BITMAP_HELVETICA_12, 0.92f, 0.98f, 1.0f);
    }
}

void desenhar_mira(Jogo *jogo)
{
    Vetor2D m = matematica_mouse_para_mundo(jogo);
    glColor4f(0.9f, 1.0f, 1.0f, 0.9f);
    glBegin(GL_LINES);
    glVertex2f(m.x - 10.0f, m.y);
    glVertex2f(m.x + 10.0f, m.y);
    glVertex2f(m.x, m.y - 10.0f);
    glVertex2f(m.x, m.y + 10.0f);
    glEnd();
    renderizar_circulo(m, 4.0f, (Cor){0.3f, 0.9f, 1.0f, 0.65f}, 12);
}

void desenhar_vida_boss(Jogo *jogo)
{
    int indice_inimigo;
    for (indice_inimigo = 0; indice_inimigo < MAXIMO_INIMIGOS; ++indice_inimigo)
    {
        Inimigo *inimigo = &jogo->inimigos[indice_inimigo];
        if (inimigo->ativo && inimigo->eh_chefao)
        {
            float percentual_vida = inimigo->vida / inimigo->vida_maxima;
            if (percentual_vida < 0.0f)
                percentual_vida = 0.0f;
            renderizar_retangulo(jogo->largura * 0.5f - 250.0f, 20.0f, 500.0f, 16.0f, (Cor){0.15f, 0.12f, 0.2f, 0.92f});
            renderizar_retangulo(jogo->largura * 0.5f - 250.0f, 20.0f, 500.0f * percentual_vida, 16.0f, (Cor){0.95f, 0.2f, 0.85f, 0.95f});
            desenhar_texto_centralizado(jogo->largura * 0.5f, 42.0f, "BOSS", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.85f, 1.0f);
            break;
        }
    }
}

void desenhar_icone_melhoria(TipoMelhoria tipo, float x, float y)
{
    switch (tipo)
    {
    case MELHORIA_DANO:
        renderizar_estrela(matematica_vetor2d(x, y), 16.0f, (Cor){1.0f, 0.7f, 0.25f, 0.95f});
        break;
    case MELHORIA_TAXA_DISPARO:
        renderizar_triangulo(matematica_vetor2d(x, y), 14.0f, (Cor){1.0f, 0.95f, 0.5f, 0.95f});
        renderizar_triangulo(matematica_vetor2d(x + 10.0f, y), 10.0f, (Cor){1.0f, 0.75f, 0.2f, 0.85f});
        break;
    case MELHORIA_VELOCIDADE:
        renderizar_losangulo(matematica_vetor2d(x - 6.0f, y), 10.0f, (Cor){0.5f, 1.0f, 0.9f, 0.95f});
        renderizar_losangulo(matematica_vetor2d(x + 8.0f, y), 8.0f, (Cor){0.3f, 0.9f, 1.0f, 0.9f});
        break;
    case MELHORIA_CURA:
        renderizar_retangulo(x - 4.0f, y - 12.0f, 8.0f, 24.0f, (Cor){0.45f, 1.0f, 0.45f, 0.95f});
        renderizar_retangulo(x - 12.0f, y - 4.0f, 24.0f, 8.0f, (Cor){0.45f, 1.0f, 0.45f, 0.95f});
        break;
    case MELHORIA_TEMPO:
        renderizar_circulo(matematica_vetor2d(x, y), 13.0f, (Cor){0.7f, 0.85f, 1.0f, 0.95f}, 16);
        renderizar_retangulo(x - 1.0f, y - 1.0f, 2.0f, 9.0f, (Cor){0.1f, 0.2f, 0.5f, 0.95f});
        renderizar_retangulo(x - 1.0f, y - 1.0f, 7.0f, 2.0f, (Cor){0.1f, 0.2f, 0.5f, 0.95f});
        break;
    case MELHORIA_GUIANCA_PP:
        renderizar_circulo(matematica_vetor2d(x, y), 10.0f, (Cor){0.2f, 0.9f, 1.0f, 0.9f}, 16);
        break;
    case MELHORIA_GUIANCA_APN:
        renderizar_circulo(matematica_vetor2d(x, y), 10.0f, (Cor){0.9f, 0.2f, 1.0f, 0.9f}, 16);
        break;
    default:
        renderizar_circulo(matematica_vetor2d(x, y), 12.0f, (Cor){1.0f, 1.0f, 1.0f, 0.8f}, 12);
        break;
    }
}

void desenhar_menu(Jogo *jogo)
{
    static float animacao_hover_menu[3] = {0.0f, 0.0f, 0.0f};
    float centro_x = jogo->largura * 0.5f;
    float escala_ui = (float)jogo->largura / 1280.0f;
    float escala_altura = (float)jogo->altura / 720.0f;
    float largura_botao, altura_botao_inicio, altura_botao_sub, margem_sombra;
    float pos_x_botao_inicio, pos_y_botao_inicio, pos_x_botao_opcoes, pos_y_botao_opcoes, pos_x_botao_placar, pos_y_botao_placar;
    float mouse_x = (float)jogo->entrada.mouse_x;
    float mouse_y = (float)jogo->entrada.mouse_y;
    float pulsacao = 0.5f + 0.5f * sinf(jogo->tempo_decorrido * 6.0f);
    int hover_botao_inicio, hover_botao_opcoes, hover_botao_placar, interface_compacta, indice_botao, s;

    if (escala_altura < escala_ui)
        escala_ui = escala_altura;
    if (escala_ui < 0.75f)
        escala_ui = 0.75f;
    if (escala_ui > 1.15f)
        escala_ui = 1.15f;
    interface_compacta = (jogo->largura < 1100 || jogo->altura < 680);

    largura_botao = 300.0f * escala_ui;
    altura_botao_inicio = 60.0f * escala_ui;
    altura_botao_sub = 50.0f * escala_ui;
    margem_sombra = 10.0f * escala_ui;

    pos_x_botao_inicio = centro_x - largura_botao * 0.5f;
    pos_y_botao_inicio = jogo->altura * 0.30f;
    pos_x_botao_opcoes = centro_x - largura_botao * 0.5f;
    pos_y_botao_opcoes = jogo->altura * 0.40f;
    pos_x_botao_placar = centro_x - largura_botao * 0.5f;
    pos_y_botao_placar = jogo->altura * 0.50f;

    hover_botao_inicio = (mouse_x >= pos_x_botao_inicio && mouse_x <= pos_x_botao_inicio + largura_botao && mouse_y >= pos_y_botao_inicio && mouse_y <= pos_y_botao_inicio + altura_botao_inicio);
    hover_botao_opcoes = (mouse_x >= pos_x_botao_opcoes && mouse_x <= pos_x_botao_opcoes + largura_botao && mouse_y >= pos_y_botao_opcoes && mouse_y <= pos_y_botao_opcoes + altura_botao_sub);
    hover_botao_placar = (mouse_x >= pos_x_botao_placar && mouse_x <= pos_x_botao_placar + largura_botao && mouse_y >= pos_y_botao_placar && mouse_y <= pos_y_botao_placar + altura_botao_sub);

    {
        float alvos_hover[3] = {hover_botao_inicio ? 1.0f : 0.0f, hover_botao_opcoes ? 1.0f : 0.0f, hover_botao_placar ? 1.0f : 0.0f};
        for (indice_botao = 0; indice_botao < 3; ++indice_botao)
        {
            animacao_hover_menu[indice_botao] += (alvos_hover[indice_botao] - animacao_hover_menu[indice_botao]) * (8.0f * jogo->tempo_delta);
            if (animacao_hover_menu[indice_botao] < 0.0f)
                animacao_hover_menu[indice_botao] = 0.0f;
            if (animacao_hover_menu[indice_botao] > 1.0f)
                animacao_hover_menu[indice_botao] = 1.0f;
        }
    }

    desenhar_texto_centralizado(centro_x, jogo->altura * 0.14f, "ORBIT SIEGE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.98f, 0.95f);
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.20f, "A platformer action game with upgrades and survival mechanics", GLUT_BITMAP_HELVETICA_12, 0.75f, 0.82f, 0.95f);
    if (!interface_compacta)
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.24f, "Press O for options or L for leaderboard", GLUT_BITMAP_HELVETICA_12, 0.72f, 0.80f, 0.92f);

    if (animacao_hover_menu[0] > 0.01f)
        renderizar_retangulo(pos_x_botao_inicio - margem_sombra - 4.0f * escala_ui, pos_y_botao_inicio - 9.0f * escala_ui,
                             largura_botao + (margem_sombra + 4.0f * escala_ui) * 2.0f, altura_botao_inicio + 18.0f * escala_ui,
                             (Cor){0.45f, 0.75f, 1.0f, (0.10f + 0.18f * pulsacao) * animacao_hover_menu[0]});
    renderizar_retangulo(pos_x_botao_inicio - margem_sombra, pos_y_botao_inicio - 5.0f * escala_ui, largura_botao + margem_sombra * 2.0f, altura_botao_inicio + 10.0f * escala_ui,
                         (Cor){0.08f + 0.12f * animacao_hover_menu[0], 0.25f + 0.20f * animacao_hover_menu[0], 0.55f + 0.35f * animacao_hover_menu[0], 0.50f + 0.05f * animacao_hover_menu[0]});
    renderizar_retangulo(pos_x_botao_inicio, pos_y_botao_inicio, largura_botao, altura_botao_inicio,
                         (Cor){0.15f + 0.07f * animacao_hover_menu[0], 0.45f + 0.13f * animacao_hover_menu[0], 0.85f + 0.15f * animacao_hover_menu[0], 0.95f + 0.03f * animacao_hover_menu[0]});
    desenhar_texto_centralizado(centro_x, pos_y_botao_inicio + 20.0f * escala_ui, "START GAME", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    desenhar_texto_centralizado(centro_x, pos_y_botao_inicio + 40.0f * escala_ui, "Press ENTER or click", GLUT_BITMAP_HELVETICA_10, 0.75f, 0.85f, 1.0f);

    if (animacao_hover_menu[1] > 0.01f)
        renderizar_retangulo(pos_x_botao_opcoes - margem_sombra - 3.0f * escala_ui, pos_y_botao_opcoes - 8.0f * escala_ui,
                             largura_botao + (margem_sombra + 3.0f * escala_ui) * 2.0f, altura_botao_sub + 16.0f * escala_ui,
                             (Cor){0.78f, 0.52f, 1.0f, (0.08f + 0.16f * pulsacao) * animacao_hover_menu[1]});
    renderizar_retangulo(pos_x_botao_opcoes - margem_sombra, pos_y_botao_opcoes - 5.0f * escala_ui, largura_botao + margem_sombra * 2.0f, altura_botao_sub + 10.0f * escala_ui,
                         (Cor){0.26f + 0.15f * animacao_hover_menu[1], 0.12f + 0.07f * animacao_hover_menu[1], 0.36f + 0.24f * animacao_hover_menu[1], 0.50f + 0.05f * animacao_hover_menu[1]});
    renderizar_retangulo(pos_x_botao_opcoes, pos_y_botao_opcoes, largura_botao, altura_botao_sub,
                         (Cor){0.36f + 0.12f * animacao_hover_menu[1], 0.22f + 0.10f * animacao_hover_menu[1], 0.62f + 0.20f * animacao_hover_menu[1], 0.90f + 0.05f * animacao_hover_menu[1]});
    desenhar_texto_centralizado(centro_x, pos_y_botao_opcoes + 17.0f * escala_ui, "OPTIONS (O)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_menu[2] > 0.01f)
        renderizar_retangulo(pos_x_botao_placar - margem_sombra - 3.0f * escala_ui, pos_y_botao_placar - 8.0f * escala_ui,
                             largura_botao + (margem_sombra + 3.0f * escala_ui) * 2.0f, altura_botao_sub + 16.0f * escala_ui,
                             (Cor){0.45f, 0.95f, 0.90f, (0.08f + 0.16f * pulsacao) * animacao_hover_menu[2]});
    renderizar_retangulo(pos_x_botao_placar - margem_sombra, pos_y_botao_placar - 5.0f * escala_ui, largura_botao + margem_sombra * 2.0f, altura_botao_sub + 10.0f * escala_ui,
                         (Cor){0.08f + 0.08f * animacao_hover_menu[2], 0.22f + 0.18f * animacao_hover_menu[2], 0.30f + 0.20f * animacao_hover_menu[2], 0.50f + 0.05f * animacao_hover_menu[2]});
    renderizar_retangulo(pos_x_botao_placar, pos_y_botao_placar, largura_botao, altura_botao_sub,
                         (Cor){0.16f + 0.10f * animacao_hover_menu[2], 0.36f + 0.16f * animacao_hover_menu[2], 0.50f + 0.18f * animacao_hover_menu[2], 0.90f + 0.05f * animacao_hover_menu[2]});
    desenhar_texto_centralizado(centro_x, pos_y_botao_placar + 17.0f * escala_ui, "LEADERBOARD (L)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (hover_botao_inicio || hover_botao_opcoes || hover_botao_placar)
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.57f, "Click to select", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.96f, 1.0f);

    renderizar_retangulo(jogo->largura * 0.5f - 400.0f, jogo->altura * 0.90f, 800.0f, interface_compacta ? 40.0f : 60.0f, (Cor){0.0f, 0.0f, 0.0f, 0.25f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.925f, "A/D or ARROWS: Move  |  SPACE or W: Jump  |  Mouse: Aim  |  LClick: Shoot", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);
    if (!interface_compacta)
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.95f, "P or ESC: Pause  |  F12: Screenshot  |  Enemy types: Orange (Standard), Diamond (Sniper), Heavy (Tank)", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);

    renderizar_retangulo(30.0f, jogo->altura * 0.78f, 320.0f, 90.0f, (Cor){0.0f, 0.0f, 0.0f, 0.4f});
    {
        char texto_stats[128];
        snprintf(texto_stats, sizeof(texto_stats), "High Score: %d", jogo->recorde_pontuacao_maxima);
        desenhar_texto(50.0f, jogo->altura * 0.82f, texto_stats, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.4f);
        snprintf(texto_stats, sizeof(texto_stats), "Max Wave: %d", jogo->recorde_onda_maxima);
        desenhar_texto(50.0f, jogo->altura * 0.86f, texto_stats, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.95f, 0.5f);
    }

    renderizar_retangulo(jogo->largura - 350.0f, jogo->altura * 0.78f, 320.0f, 90.0f, (Cor){0.0f, 0.0f, 0.0f, 0.4f});
    {
        char texto_config[128];
        snprintf(texto_config, sizeof(texto_config), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
        desenhar_texto(jogo->largura - 330.0f, jogo->altura * 0.82f, texto_config, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.88f, 0.3f);
        snprintf(texto_config, sizeof(texto_config), "Audio: %s", jogo->audio_habilitado ? "ON" : "OFF");
        desenhar_texto(jogo->largura - 330.0f, jogo->altura * 0.86f, texto_config, GLUT_BITMAP_HELVETICA_18, 0.3f, jogo->audio_habilitado ? 0.9f : 0.5f, 0.8f);
    }

    renderizar_retangulo(jogo->largura * 0.5f - 280.0f, jogo->altura * 0.60f, 560.0f, 140.0f, (Cor){0.05f, 0.08f, 0.15f, 0.7f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.63f, "TOP 5 SCORES", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.9f, 0.5f);
    for (s = 0; s < jogo->numero_melhores_pontuacoes && s < 5; ++s)
    {
        char linha_pontuacao[128];
        snprintf(linha_pontuacao, sizeof(linha_pontuacao), "%d. %-18s  %7d  W%d", s + 1, jogo->melhores_pontuacoes[s].nome, jogo->melhores_pontuacoes[s].pontuacao, jogo->melhores_pontuacoes[s].onda);
        desenhar_texto(jogo->largura * 0.5f - 260.0f, jogo->altura * 0.67f + s * 20.0f, linha_pontuacao, GLUT_BITMAP_HELVETICA_10, 0.8f, 0.88f, 0.98f);
    }
}

void desenhar_placar(Jogo *jogo)
{
    static float animacao_hover_placar[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float centro_x = jogo->largura * 0.5f;
    float mouse_x = (float)jogo->entrada.mouse_x;
    float mouse_y = (float)jogo->entrada.mouse_y;
    float pulsacao = 0.5f + 0.5f * sinf(jogo->tempo_decorrido * 6.0f);
    int hover_botao_anterior, hover_botao_proximo, hover_botao_limpar, hover_botao_voltar, indice_hover;
    int total_paginas = (jogo->numero_todas_pontuacoes + jogo->itens_por_pagina_pontuacao - 1) / jogo->itens_por_pagina_pontuacao;
    int indice_inicio_pagina = jogo->pagina_pontuacao * jogo->itens_por_pagina_pontuacao;
    int indice_fim_pagina = indice_inicio_pagina + jogo->itens_por_pagina_pontuacao;
    char texto_pagina[64];

    hover_botao_anterior = (mouse_x >= centro_x - 255.0f && mouse_x <= centro_x - 95.0f && mouse_y >= jogo->altura * 0.20f && mouse_y <= jogo->altura * 0.20f + 44.0f);
    hover_botao_proximo = (mouse_x >= centro_x + 95.0f && mouse_x <= centro_x + 255.0f && mouse_y >= jogo->altura * 0.20f && mouse_y <= jogo->altura * 0.20f + 44.0f);
    hover_botao_limpar = (mouse_x >= centro_x - 110.0f && mouse_x <= centro_x + 110.0f && mouse_y >= jogo->altura * 0.20f && mouse_y <= jogo->altura * 0.20f + 44.0f);
    hover_botao_voltar = (mouse_x >= centro_x - 110.0f && mouse_x <= centro_x + 110.0f && mouse_y >= jogo->altura * 0.11f && mouse_y <= jogo->altura * 0.11f + 44.0f);

    {
        float alvos_hover[4] = {hover_botao_anterior ? 1.0f : 0.0f, hover_botao_proximo ? 1.0f : 0.0f, hover_botao_limpar ? 1.0f : 0.0f, hover_botao_voltar ? 1.0f : 0.0f};
        for (indice_hover = 0; indice_hover < 4; ++indice_hover)
        {
            animacao_hover_placar[indice_hover] += (alvos_hover[indice_hover] - animacao_hover_placar[indice_hover]) * (8.0f * jogo->tempo_delta);
            if (animacao_hover_placar[indice_hover] < 0.0f)
                animacao_hover_placar[indice_hover] = 0.0f;
            if (animacao_hover_placar[indice_hover] > 1.0f)
                animacao_hover_placar[indice_hover] = 1.0f;
        }
    }

    if (total_paginas <= 0)
        total_paginas = 1;
    if (indice_fim_pagina > jogo->numero_todas_pontuacoes)
        indice_fim_pagina = jogo->numero_todas_pontuacoes;

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.0f, 0.0f, 0.0f, 0.67f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.82f, "SCOREBOARD", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    renderizar_retangulo(jogo->largura * 0.5f - 300.0f, jogo->altura * 0.26f, 600.0f, 360.0f, (Cor){0.06f, 0.09f, 0.17f, 0.88f});
    desenhar_texto(jogo->largura * 0.5f - 270.0f, jogo->altura * 0.58f, "#   NAME                    SCORE       WAVE", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

    if (jogo->numero_todas_pontuacoes == 0)
    {
        desenhar_texto(jogo->largura * 0.5f - 96.0f, jogo->altura * 0.45f, "No scores saved yet", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.7f);
    }
    else
    {
        int indice_pontuacao;
        for (indice_pontuacao = indice_inicio_pagina; indice_pontuacao < indice_fim_pagina; ++indice_pontuacao)
        {
            char linha_pontuacao[128];
            int pos_y_linha = (int)(jogo->altura * 0.53f - (indice_pontuacao - indice_inicio_pagina) * 28.0f);
            snprintf(linha_pontuacao, sizeof(linha_pontuacao), "%02d  %-22s %8d   %4d", indice_pontuacao + 1, jogo->todas_pontuacoes[indice_pontuacao].nome, jogo->todas_pontuacoes[indice_pontuacao].pontuacao, jogo->todas_pontuacoes[indice_pontuacao].onda);
            desenhar_texto(jogo->largura * 0.5f - 270.0f, (float)pos_y_linha, linha_pontuacao, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.93f, 1.0f);
        }
    }

    snprintf(texto_pagina, sizeof(texto_pagina), "Page %d/%d", jogo->pagina_pontuacao + 1, total_paginas);
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.22f, texto_pagina, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);

    if (animacao_hover_placar[0] > 0.01f)
        renderizar_retangulo(centro_x - 258.0f, jogo->altura * 0.20f - 3.0f, 166.0f, 50.0f, (Cor){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_placar[0]});
    renderizar_retangulo(jogo->largura * 0.5f - 255.0f, jogo->altura * 0.20f, 160.0f, 44.0f,
                         (Cor){0.20f + 0.08f * animacao_hover_placar[0], 0.30f + 0.10f * animacao_hover_placar[0], 0.52f + 0.16f * animacao_hover_placar[0], 0.90f + 0.05f * animacao_hover_placar[0]});
    desenhar_texto_centralizado(centro_x - 175.0f, jogo->altura * 0.228f, "PREV", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_placar[1] > 0.01f)
        renderizar_retangulo(centro_x + 92.0f, jogo->altura * 0.20f - 3.0f, 166.0f, 50.0f, (Cor){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_placar[1]});
    renderizar_retangulo(jogo->largura * 0.5f + 95.0f, jogo->altura * 0.20f, 160.0f, 44.0f,
                         (Cor){0.20f + 0.08f * animacao_hover_placar[1], 0.30f + 0.10f * animacao_hover_placar[1], 0.52f + 0.16f * animacao_hover_placar[1], 0.90f + 0.05f * animacao_hover_placar[1]});
    desenhar_texto_centralizado(centro_x + 175.0f, jogo->altura * 0.228f, "NEXT", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_placar[2] > 0.01f)
        renderizar_retangulo(centro_x - 113.0f, jogo->altura * 0.20f - 3.0f, 226.0f, 50.0f, (Cor){1.0f, 0.52f, 0.54f, (0.10f + 0.12f * pulsacao) * animacao_hover_placar[2]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.20f, 220.0f, 44.0f,
                         (Cor){0.45f + 0.10f * animacao_hover_placar[2], 0.20f + 0.06f * animacao_hover_placar[2], 0.22f + 0.08f * animacao_hover_placar[2], 0.90f + 0.05f * animacao_hover_placar[2]});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.228f, "CLEAR (C)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_placar[3] > 0.01f)
        renderizar_retangulo(centro_x - 113.0f, jogo->altura * 0.11f - 3.0f, 226.0f, 50.0f, (Cor){0.52f, 0.78f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_placar[3]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.11f, 220.0f, 44.0f,
                         (Cor){0.10f + 0.08f * animacao_hover_placar[3], 0.35f + 0.12f * animacao_hover_placar[3], 0.70f + 0.14f * animacao_hover_placar[3], 0.90f + 0.05f * animacao_hover_placar[3]});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.138f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void desenhar_opcoes(Jogo *jogo)
{
    static float animacao_hover_opcoes[3] = {0.0f, 0.0f, 0.0f};
    float centro_x = jogo->largura * 0.5f;
    float mouse_x = (float)jogo->entrada.mouse_x;
    float mouse_y = (float)jogo->entrada.mouse_y;
    float pulsacao = 0.5f + 0.5f * sinf(jogo->tempo_decorrido * 6.0f);
    int hover_botao_audio = (mouse_x >= centro_x - 180.0f && mouse_x <= centro_x + 180.0f && mouse_y >= jogo->altura * 0.56f && mouse_y <= jogo->altura * 0.56f + 56.0f);
    int hover_botao_dificuldade = (mouse_x >= centro_x - 180.0f && mouse_x <= centro_x + 180.0f && mouse_y >= jogo->altura * 0.46f && mouse_y <= jogo->altura * 0.46f + 56.0f);
    int hover_botao_voltar = (mouse_x >= centro_x - 110.0f && mouse_x <= centro_x + 110.0f && mouse_y >= jogo->altura * 0.31f && mouse_y <= jogo->altura * 0.31f + 50.0f);
    int indice_opcao;
    char texto_opcao[128];

    {
        float alvos_hover[3] = {hover_botao_audio ? 1.0f : 0.0f, hover_botao_dificuldade ? 1.0f : 0.0f, hover_botao_voltar ? 1.0f : 0.0f};
        for (indice_opcao = 0; indice_opcao < 3; ++indice_opcao)
        {
            animacao_hover_opcoes[indice_opcao] += (alvos_hover[indice_opcao] - animacao_hover_opcoes[indice_opcao]) * (8.0f * jogo->tempo_delta);
            if (animacao_hover_opcoes[indice_opcao] < 0.0f)
                animacao_hover_opcoes[indice_opcao] = 0.0f;
            if (animacao_hover_opcoes[indice_opcao] > 1.0f)
                animacao_hover_opcoes[indice_opcao] = 1.0f;
        }
    }

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.0f, 0.0f, 0.0f, 0.62f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.73f, "OPTIONS", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_opcoes[0] > 0.01f)
        renderizar_retangulo(centro_x - 183.0f, jogo->altura * 0.56f - 3.0f, 366.0f, 62.0f, (Cor){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_opcoes[0]});
    renderizar_retangulo(jogo->largura * 0.5f - 180.0f, jogo->altura * 0.56f, 360.0f, 56.0f,
                         (Cor){0.20f + 0.08f * animacao_hover_opcoes[0], 0.25f + 0.10f * animacao_hover_opcoes[0], 0.50f + 0.16f * animacao_hover_opcoes[0], 0.90f + 0.05f * animacao_hover_opcoes[0]});
    snprintf(texto_opcao, sizeof(texto_opcao), "Audio: %s  (click or A)", jogo->audio_habilitado ? "ON" : "OFF");
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.595f, texto_opcao, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (animacao_hover_opcoes[1] > 0.01f)
        renderizar_retangulo(centro_x - 183.0f, jogo->altura * 0.46f - 3.0f, 366.0f, 62.0f, (Cor){0.84f, 0.66f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_opcoes[1]});
    renderizar_retangulo(jogo->largura * 0.5f - 180.0f, jogo->altura * 0.46f, 360.0f, 56.0f,
                         (Cor){0.28f + 0.10f * animacao_hover_opcoes[1], 0.22f + 0.08f * animacao_hover_opcoes[1], 0.44f + 0.15f * animacao_hover_opcoes[1], 0.90f + 0.05f * animacao_hover_opcoes[1]});
    snprintf(texto_opcao, sizeof(texto_opcao), "Difficulty: %s  (click or D)", inimigo_nome_dificuldade(jogo->dificuldade));
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.495f, texto_opcao, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    desenhar_texto_centralizado(centro_x, jogo->altura * 0.40f, "Easy: less damage + more time | Hard: stronger/faster enemies", GLUT_BITMAP_HELVETICA_12, 0.92f, 0.92f, 0.92f);

    if (animacao_hover_opcoes[2] > 0.01f)
        renderizar_retangulo(centro_x - 113.0f, jogo->altura * 0.31f - 3.0f, 226.0f, 56.0f, (Cor){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_opcoes[2]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.31f, 220.0f, 50.0f,
                         (Cor){0.10f + 0.08f * animacao_hover_opcoes[2], 0.35f + 0.12f * animacao_hover_opcoes[2], 0.70f + 0.14f * animacao_hover_opcoes[2], 0.90f + 0.05f * animacao_hover_opcoes[2]});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.342f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void desenhar_pausa(Jogo *jogo)
{
    float centro_x = jogo->largura * 0.5f;
    int interface_compacta = (jogo->largura < 1100 || jogo->altura < 680);
    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.0f, 0.0f, 0.0f, 0.56f});
    renderizar_retangulo(centro_x - 240.0f, jogo->altura * 0.42f, 480.0f, interface_compacta ? 110.0f : 140.0f, (Cor){0.06f, 0.10f, 0.18f, 0.88f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.47f, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.53f, "Press P or ENTER to continue", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
    if (!interface_compacta)
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.58f, "ESC also resumes the game", GLUT_BITMAP_HELVETICA_12, 0.78f, 0.86f, 0.98f);
}

void desenhar_melhorias_tela(Jogo *jogo)
{
    static float animacao_hover_melhoria[MAXIMO_OPCOES_UPGRADE] = {0.0f, 0.0f, 0.0f};
    static float animacao_hover_rerolar = 0.0f;
    Vetor2D posicao_mouse = matematica_vetor2d((float)jogo->entrada.mouse_x, (float)jogo->entrada.mouse_y);
    float centro_x = jogo->largura * 0.5f;
    float escala_ui = (float)jogo->largura / 1280.0f;
    float escala_altura = (float)jogo->altura / 720.0f;
    float largura_cartao, altura_cartao, espacamento_cartao, largura_total_cartoes, pos_x_inicio_cartoes, pos_y_cartoes;
    float largura_rerolar, altura_rerolar, pos_x_rerolar, pos_y_rerolar;
    float pulsacao = 0.5f + 0.5f * sinf(jogo->tempo_decorrido * 6.0f);
    int interface_compacta, indice_opcao;

    if (escala_altura < escala_ui)
        escala_ui = escala_altura;
    if (escala_ui < 0.78f)
        escala_ui = 0.78f;
    if (escala_ui > 1.08f)
        escala_ui = 1.08f;
    interface_compacta = (jogo->largura < 1100 || jogo->altura < 680);

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

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.0f, 0.0f, 0.0f, 0.58f});
    renderizar_retangulo(centro_x - largura_total_cartoes * 0.5f - 40.0f * escala_ui, pos_y_cartoes - 70.0f * escala_ui,
                         largura_total_cartoes + 80.0f * escala_ui, altura_cartao + 190.0f * escala_ui, (Cor){0.07f, 0.10f, 0.18f, 0.86f});
    desenhar_texto_centralizado(centro_x, pos_y_cartoes - 34.0f * escala_ui, "CHOOSE UPGRADE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    if (!interface_compacta)
        desenhar_texto_centralizado(centro_x, pos_y_cartoes - 12.0f * escala_ui, "Select with mouse click or keys 1, 2, 3", GLUT_BITMAP_HELVETICA_12, 0.80f, 0.88f, 1.0f);
    jogo->melhoria_selecionada = -1;

    for (indice_opcao = 0; indice_opcao < MAXIMO_OPCOES_UPGRADE; ++indice_opcao)
    {
        float pos_x_cartao = pos_x_inicio_cartoes + indice_opcao * (largura_cartao + espacamento_cartao);
        float pos_y_cartao = pos_y_cartoes;
        int cartao_sob_cursor = (posicao_mouse.x >= pos_x_cartao && posicao_mouse.x <= pos_x_cartao + largura_cartao && posicao_mouse.y >= pos_y_cartao && posicao_mouse.y <= pos_y_cartao + altura_cartao);
        if (cartao_sob_cursor)
            jogo->melhoria_selecionada = indice_opcao;
        animacao_hover_melhoria[indice_opcao] += ((cartao_sob_cursor ? 1.0f : 0.0f) - animacao_hover_melhoria[indice_opcao]) * (8.0f * jogo->tempo_delta);
        if (animacao_hover_melhoria[indice_opcao] < 0.0f)
            animacao_hover_melhoria[indice_opcao] = 0.0f;
        if (animacao_hover_melhoria[indice_opcao] > 1.0f)
            animacao_hover_melhoria[indice_opcao] = 1.0f;

        if (animacao_hover_melhoria[indice_opcao] > 0.01f)
            renderizar_retangulo(pos_x_cartao - 4.0f * escala_ui, pos_y_cartao - 4.0f * escala_ui, largura_cartao + 8.0f * escala_ui, altura_cartao + 8.0f * escala_ui,
                                 (Cor){0.65f, 0.88f, 1.0f, (0.10f + 0.14f * pulsacao) * animacao_hover_melhoria[indice_opcao]});
        renderizar_retangulo(pos_x_cartao, pos_y_cartao, largura_cartao, altura_cartao,
                             (Cor){0.14f + 0.06f * animacao_hover_melhoria[indice_opcao], 0.20f + indice_opcao * 0.07f + 0.06f * animacao_hover_melhoria[indice_opcao], 0.45f + 0.10f * animacao_hover_melhoria[indice_opcao], 0.92f + 0.04f * animacao_hover_melhoria[indice_opcao]});
        desenhar_icone_melhoria(jogo->melhorias[indice_opcao].tipo, pos_x_cartao + largura_cartao - 25.0f * escala_ui, pos_y_cartao + altura_cartao - 26.0f * escala_ui);
        desenhar_texto_centralizado(pos_x_cartao + largura_cartao * 0.5f, pos_y_cartao + 34.0f * escala_ui, jogo->melhorias[indice_opcao].rotulo, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        desenhar_texto_limites(pos_x_cartao + 12.0f * escala_ui, pos_y_cartao + 62.0f * escala_ui, jogo->melhorias[indice_opcao].descricao, GLUT_BITMAP_HELVETICA_12, 0.88f, 0.95f, 1.0f, largura_cartao - 32.0f * escala_ui);
        {
            char numero_indice[8];
            snprintf(numero_indice, sizeof(numero_indice), "[%d]", indice_opcao + 1);
            desenhar_texto(pos_x_cartao + 12.0f * escala_ui, pos_y_cartao + altura_cartao - 14.0f * escala_ui, numero_indice, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.95f, 0.6f);
        }
    }

    {
        int rerolar_sob_cursor = (posicao_mouse.x >= pos_x_rerolar && posicao_mouse.x <= pos_x_rerolar + largura_rerolar && posicao_mouse.y >= pos_y_rerolar && posicao_mouse.y <= pos_y_rerolar + altura_rerolar);
        animacao_hover_rerolar += ((rerolar_sob_cursor ? 1.0f : 0.0f) - animacao_hover_rerolar) * (8.0f * jogo->tempo_delta);
        if (animacao_hover_rerolar < 0.0f)
            animacao_hover_rerolar = 0.0f;
        if (animacao_hover_rerolar > 1.0f)
            animacao_hover_rerolar = 1.0f;

        if (animacao_hover_rerolar > 0.01f)
            renderizar_retangulo(pos_x_rerolar - 3.0f * escala_ui, pos_y_rerolar - 3.0f * escala_ui, largura_rerolar + 6.0f * escala_ui, altura_rerolar + 6.0f * escala_ui,
                                 (Cor){0.75f, 0.64f, 1.0f, (0.08f + 0.12f * pulsacao) * animacao_hover_rerolar});
        renderizar_retangulo(pos_x_rerolar, pos_y_rerolar, largura_rerolar, altura_rerolar,
                             (Cor){0.22f + 0.10f * animacao_hover_rerolar, 0.18f + 0.08f * animacao_hover_rerolar, 0.35f + 0.12f * animacao_hover_rerolar, 0.92f + 0.04f * animacao_hover_rerolar});
        desenhar_texto_centralizado(centro_x, pos_y_rerolar + 26.0f * escala_ui, "Reroll (R) - Cost: 3 gold", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.95f, 0.9f);
    }

    if (jogo->melhoria_selecionada >= 0 && jogo->melhoria_selecionada < MAXIMO_OPCOES_UPGRADE)
    {
        renderizar_retangulo(centro_x - 240.0f * escala_ui, pos_y_rerolar + altura_rerolar + 12.0f * escala_ui, 480.0f * escala_ui, 40.0f * escala_ui, (Cor){0.08f, 0.12f, 0.24f, 0.85f});
        desenhar_texto_centralizado(centro_x, pos_y_rerolar + altura_rerolar + 36.0f * escala_ui, jogo->melhorias[jogo->melhoria_selecionada].descricao, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);
    }
}

void desenhar_fim(Jogo *jogo)
{
    static float animacao_hover_menu_fim = 0.0f;
    const char *texto_titulo = (jogo->tela == TELA_VITORIA) ? "VICTORY" : "DEFEAT";
    float centro_x = jogo->largura * 0.5f;
    float mouse_x = (float)jogo->entrada.mouse_x;
    float mouse_y = (float)jogo->entrada.mouse_y;
    float pulsacao = 0.5f + 0.5f * sinf(jogo->tempo_decorrido * 6.0f);
    int hover_botao_menu = (jogo->nome_salvo && mouse_x >= centro_x - 110.0f && mouse_x <= centro_x + 110.0f && mouse_y >= jogo->altura * 0.45f && mouse_y <= jogo->altura * 0.45f + 50.0f);
    char texto_linha[160];

    animacao_hover_menu_fim += ((hover_botao_menu ? 1.0f : 0.0f) - animacao_hover_menu_fim) * (8.0f * jogo->tempo_delta);
    if (animacao_hover_menu_fim < 0.0f)
        animacao_hover_menu_fim = 0.0f;
    if (animacao_hover_menu_fim > 1.0f)
        animacao_hover_menu_fim = 1.0f;

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.0f, 0.0f, 0.0f, 0.62f});
    renderizar_retangulo(centro_x - 280.0f, jogo->altura * 0.38f, 560.0f, 260.0f, (Cor){0.08f, 0.10f, 0.18f, 0.86f});
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.44f, texto_titulo, GLUT_BITMAP_TIMES_ROMAN_24,
                                jogo->tela == TELA_VITORIA ? 0.9f : 1.0f,
                                jogo->tela == TELA_VITORIA ? 1.0f : 0.85f,
                                jogo->tela == TELA_VITORIA ? 0.9f : 0.85f);

    snprintf(texto_linha, sizeof(texto_linha), "Score: %d  |  Wave: %d", jogo->pontuacao, jogo->onda);
    desenhar_texto_centralizado(centro_x, jogo->altura * 0.50f, texto_linha, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

    if (!jogo->nome_salvo)
    {
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.57f, "Type name and press ENTER to save score:", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
        renderizar_retangulo(centro_x - 170.0f, jogo->altura * 0.61f, 340.0f, 40.0f, (Cor){0.12f, 0.12f, 0.2f, 0.9f});
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.635f, jogo->nome_jogador, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 0.8f);
    }
    else
    {
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.57f, "Saved. Press ENTER or M to go menu.", GLUT_BITMAP_HELVETICA_18, 0.95f, 1.0f, 0.85f);
        if (animacao_hover_menu_fim > 0.01f)
            renderizar_retangulo(centro_x - 113.0f, jogo->altura * 0.45f - 3.0f, 226.0f, 56.0f,
                                 (Cor){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulsacao) * animacao_hover_menu_fim});
        renderizar_retangulo(centro_x - 110.0f, jogo->altura * 0.45f, 220.0f, 50.0f,
                             (Cor){0.10f + 0.10f * animacao_hover_menu_fim, 0.35f + 0.12f * animacao_hover_menu_fim, 0.70f + 0.14f * animacao_hover_menu_fim, 0.90f + 0.05f * animacao_hover_menu_fim});
        desenhar_texto_centralizado(centro_x, jogo->altura * 0.482f, "MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }
}

void desenhar_flash(Jogo *jogo)
{
    if (jogo->tela == TELA_JOGANDO && jogo->flash_dano > 0.0f)
        renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.9f, 0.1f, 0.1f, 0.18f * jogo->flash_dano});

    if (jogo->tela == TELA_JOGANDO && jogo->flash_melhoria > 0.0f)
        renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.2f, 0.8f, 1.0f, 0.12f * jogo->flash_melhoria});
}
