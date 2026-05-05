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

void entrada_tecla_pressionada(Game *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;

    if (tecla < 256)
    {
        jogo->input.keys[tecla] = 1;
        jogo->input.keysPressed[tecla] = 1;
    }

    if (tecla == 27)
    {
        if (jogo->screen == SCREEN_PLAYING)
            jogo->screen = SCREEN_PAUSED;
        else if (jogo->screen == SCREEN_PAUSED)
            jogo->screen = SCREEN_PLAYING;
        else if (jogo->screen == SCREEN_OPTIONS)
            jogo->screen = SCREEN_MENU;
        else if (jogo->screen == SCREEN_SCORES)
            jogo->screen = SCREEN_MENU;
    }

    if ((tecla == 'p' || tecla == 'P') && jogo->screen == SCREEN_PLAYING)
        jogo->screen = SCREEN_PAUSED;
    else if ((tecla == 'p' || tecla == 'P') && jogo->screen == SCREEN_PAUSED)
        jogo->screen = SCREEN_PLAYING;

    if (jogo->screen == SCREEN_MENU && (tecla == 13 || tecla == ' '))
        jogo_reiniciar(jogo);

    if (jogo->screen == SCREEN_MENU && (tecla == 'o' || tecla == 'O'))
        jogo->screen = SCREEN_OPTIONS;

    if (jogo->screen == SCREEN_MENU && (tecla == 'l' || tecla == 'L'))
        jogo->screen = SCREEN_SCORES;

    if (jogo->screen == SCREEN_PAUSED && tecla == 13)
        jogo->screen = SCREEN_PLAYING;

    if (jogo->screen == SCREEN_UPGRADE)
    {
        if (tecla == '1' || tecla == '2' || tecla == '3')
        {
            int idx = tecla - '1';
            if (idx >= 0 && idx < MAXIMO_OPCOES_UPGRADE)
                melhorias_escolher(jogo, idx);
        }
        else if (tecla == 'r' || tecla == 'R')
        {
            if (jogo->gold >= 3)
            {
                jogo->gold -= 3;
                melhorias_rolar_opcoes(jogo);
                interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            }
            else
            {
                interface_notificar(jogo, "Not enough gold for reroll");
            }
        }
    }

    if (jogo->screen == SCREEN_OPTIONS)
    {
        if (tecla == 'a' || tecla == 'A')
        {
            jogo->audioEnabled = !jogo->audioEnabled;
            audio_definir_ativacao(jogo->audioEnabled);
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            interface_notificar(jogo, jogo->audioEnabled ? "Audio ON" : "Audio OFF");
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->difficulty = (jogo->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->difficulty));
                interface_notificar(jogo, msg);
            }
        }
        else if (tecla == 13 || tecla == 'm' || tecla == 'M')
        {
            jogo->screen = SCREEN_MENU;
        }
    }

    if (jogo->screen == SCREEN_SCORES)
    {
        int pageCount = (jogo->allScoreCount + jogo->scorePageSize - 1) / jogo->scorePageSize;
        if (pageCount <= 0)
            pageCount = 1;

        if (tecla == 'a' || tecla == 'A')
        {
            jogo->scorePage--;
            if (jogo->scorePage < 0)
                jogo->scorePage = 0;
        }
        else if (tecla == 'd' || tecla == 'D')
        {
            jogo->scorePage++;
            if (jogo->scorePage > pageCount - 1)
                jogo->scorePage = pageCount - 1;
        }
        else if (tecla == 'c' || tecla == 'C')
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->scorePage = 0;
            interface_notificar(jogo, "Score history cleared");
        }
        else if (tecla == 'm' || tecla == 'M' || tecla == 13)
        {
            jogo->screen = SCREEN_MENU;
        }
    }

    if ((jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE) && jogo->enteringName)
    {
        int len = (int)strlen(jogo->playerName);

        if (tecla == 8 && len > 0)
        {
            jogo->playerName[len - 1] = '\0';
            return;
        }

        if (tecla == 13 && !jogo->nameSaved)
        {
            if (strlen(jogo->playerName) == 0)
                snprintf(jogo->playerName, sizeof(jogo->playerName), "Player");
            persistencia_apor_pontuacao(jogo->playerName, jogo->score, jogo->wave);
            jogo->nameSaved = 1;
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            interface_notificar(jogo, "Score saved");
            return;
        }

        if ((isalnum(tecla) || tecla == ' ' || tecla == '_') && len < (int)sizeof(jogo->playerName) - 1)
        {
            jogo->playerName[len] = (char)tecla;
            jogo->playerName[len + 1] = '\0';
        }

        if (jogo->nameSaved && (tecla == 'm' || tecla == 'M' || tecla == 13))
        {
            jogo->screen = SCREEN_MENU;
            jogo->enteringName = 0;
        }
    }
}

void entrada_tecla_levantada(Game *jogo, unsigned char tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->input.keys[tecla] = 0;
}

void entrada_especial_pressionado(Game *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->input.special[tecla] = 1;

    if (tecla == GLUT_KEY_F12)
    {
        time_t t = time(NULL);
        struct tm *tmv = localtime(&t);
        char filename[128];
        snprintf(filename, sizeof(filename), "assets/screenshots/shot_%04d%02d%02d_%02d%02d%02d.ppm",
                 tmv->tm_year + 1900, tmv->tm_mon + 1, tmv->tm_mday,
                 tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
        if (imagem_salvar_ppm(filename, jogo->width, jogo->height))
            interface_notificar(jogo, "Screenshot saved");
        else
            interface_notificar(jogo, "Screenshot failed");
    }
}

void entrada_especial_levantado(Game *jogo, int tecla, int x, int y)
{
    (void)x;
    (void)y;
    if (tecla < 256)
        jogo->input.special[tecla] = 0;
}

void entrada_mouse_pressionado(Game *jogo, int botao, int estado, int x, int y)
{
    float wx;
    float wy;
    float bx;
    float by;

    jogo->input.mouseX = x;
    jogo->input.mouseY = y;

    if (botao >= 0 && botao < 3)
    {
        if (estado == GLUT_DOWN)
        {
            jogo->input.mouseDown[botao] = 1;
            jogo->input.mousePressed[botao] = 1;
        }
        else
        {
            jogo->input.mouseDown[botao] = 0;
        }
    }

    wx = (float)x;
    wy = (float)y;

    if (botao != GLUT_LEFT_BUTTON || estado != GLUT_DOWN)
        return;

    if (jogo->screen == SCREEN_MENU)
    {
        float uiScale = (float)jogo->width / 1280.0f;
        float hScale = (float)jogo->height / 720.0f;
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
        bx = jogo->width * 0.5f - btnW * 0.5f;
        by = jogo->height * 0.30f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + startH)
        {
            jogo_reiniciar(jogo);
            return;
        }

        by = jogo->height * 0.40f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->screen = SCREEN_OPTIONS;
            return;
        }

        by = jogo->height * 0.50f;
        if (wx >= bx && wx <= bx + btnW && wy >= by && wy <= by + subH)
        {
            jogo->screen = SCREEN_SCORES;
            return;
        }
    }

    if (jogo->screen == SCREEN_OPTIONS)
    {
        bx = jogo->width * 0.5f - 180.0f;
        by = jogo->height * 0.56f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->audioEnabled = !jogo->audioEnabled;
            audio_definir_ativacao(jogo->audioEnabled);
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            interface_notificar(jogo, jogo->audioEnabled ? "Audio ON" : "Audio OFF");
            return;
        }

        by = jogo->height * 0.46f;
        if (wx >= bx && wx <= bx + 360.0f && wy >= by && wy <= by + 56.0f)
        {
            jogo->difficulty = (jogo->difficulty + 1) % 3;
            persistencia_salvar_configuracoes(jogo->audioEnabled, jogo->difficulty);
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->difficulty));
                interface_notificar(jogo, msg);
            }
            return;
        }

        bx = jogo->width * 0.5f - 110.0f;
        by = jogo->height * 0.31f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->screen = SCREEN_MENU;
            return;
        }
    }

    if (jogo->screen == SCREEN_SCORES)
    {
        int pageCount = (jogo->allScoreCount + jogo->scorePageSize - 1) / jogo->scorePageSize;
        if (pageCount <= 0)
            pageCount = 1;

        bx = jogo->width * 0.5f - 255.0f;
        by = jogo->height * 0.20f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->scorePage--;
            if (jogo->scorePage < 0)
                jogo->scorePage = 0;
            return;
        }

        bx = jogo->width * 0.5f + 95.0f;
        if (wx >= bx && wx <= bx + 160.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->scorePage++;
            if (jogo->scorePage > pageCount - 1)
                jogo->scorePage = pageCount - 1;
            return;
        }

        bx = jogo->width * 0.5f - 110.0f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            persistencia_limpar_pontuacoes();
            interface_refrescar_pontuacoes_maximas(jogo);
            interface_refrescar_pontuacoes(jogo);
            jogo->scorePage = 0;
            interface_notificar(jogo, "Score history cleared");
            return;
        }

        by = jogo->height * 0.11f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 44.0f)
        {
            jogo->screen = SCREEN_MENU;
            return;
        }
    }

    if (jogo->screen == SCREEN_UPGRADE)
    {
        float uiScale = (float)jogo->width / 1280.0f;
        float hScale = (float)jogo->height / 720.0f;
        float centerX = jogo->width * 0.5f;
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
        cardY = jogo->height * 0.36f;

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
        if (wx >= bx && wx <= bx + rerollW && wy >= by && wy <= by + rerollH && jogo->gold >= 3)
        {
            jogo->gold -= 3;
            melhorias_rolar_opcoes(jogo);
            interface_notificar(jogo, "Upgrade rerolled (-3 gold)");
            return;
        }
    }

    if ((jogo->screen == SCREEN_WIN || jogo->screen == SCREEN_LOSE) && jogo->nameSaved)
    {
        bx = jogo->width * 0.5f - 110.0f;
        by = jogo->height * 0.45f;
        if (wx >= bx && wx <= bx + 220.0f && wy >= by && wy <= by + 50.0f)
        {
            jogo->screen = SCREEN_MENU;
            jogo->enteringName = 0;
        }
    }
}

void entrada_mouse_movido(Game *jogo, int x, int y)
{
    jogo->input.mouseX = x;
    jogo->input.mouseY = y;
}

void entrada_iniciar_frame(Game *jogo)
{
    memset(jogo->input.keysPressed, 0, sizeof(jogo->input.keysPressed));
    memset(jogo->input.mousePressed, 0, sizeof(jogo->input.mousePressed));
}
