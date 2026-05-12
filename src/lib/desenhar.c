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

void desenhar_texto_limites(float x, float y, const char *texto, void *fonte, float r, float g, float b, float largMax)
{
    const unsigned char *p = (const unsigned char *)texto;
    float curX = x;
    float curY = y;
    char palavra[64];
    int idxPalavra = 0;
    glColor3f(r, g, b);

    while (*p)
    {
        if (*p == ' ' || *p == '\0')
        {
            palavra[idxPalavra] = '\0';
            float largPalavra = 0;
            for (int i = 0; i < idxPalavra; i++)
                largPalavra += glutBitmapWidth(fonte, palavra[i]);

            if (curX + largPalavra > x + largMax)
            {
                curX = x;
                curY += 15.0f;
            }

            glRasterPos2f(curX, curY);
            for (int i = 0; i < idxPalavra; i++)
                glutBitmapCharacter(fonte, palavra[i]);
            curX += largPalavra + glutBitmapWidth(fonte, ' ');
            idxPalavra = 0;
            if (*p == '\0')
                break;
        }
        else
        {
            if (idxPalavra < 63)
                palavra[idxPalavra++] = *p;
        }
        p++;
    }
    if (idxPalavra > 0)
    {
        palavra[idxPalavra] = '\0';
        glRasterPos2f(curX, curY);
        for (int i = 0; i < idxPalavra; i++)
            glutBitmapCharacter(fonte, palavra[i]);
    }
}

void desenhar_vida_boss(Jogo *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_INIMIGOS; ++i)
    {
        Inimigo *e = &jogo->inimigos[i];
        if (e->ativo && e->ehChefao)
        {
            float pct = e->vida / e->vidaMaxima;
            if (pct < 0.0f)
                pct = 0.0f;
            renderizar_retangulo(jogo->largura * 0.5f - 250.0f, 20.0f, 500.0f, 16.0f, (Color){0.15f, 0.12f, 0.2f, 0.92f});
            renderizar_retangulo(jogo->largura * 0.5f - 250.0f, 20.0f, 500.0f * pct, 16.0f, (Color){0.95f, 0.2f, 0.85f, 0.95f});
            desenhar_texto(jogo->largura * 0.5f - 56.0f, 42.0f, "BOSS", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.85f, 1.0f);
            break;
        }
    }
}

void desenhar_hud(Jogo *jogo)
{
    char linha[128];

    renderizar_retangulo(15.0f, 52.0f, 450.0f, 62.0f, (Color){0.0f, 0.0f, 0.0f, 0.35f});
    snprintf(linha, sizeof(linha), "HP: %.0f/%.0f   Time: %.0f   Score: %d   Wave: %d   Gold: %d",
             jogo->jogador.vida, jogo->jogador.vidaMaxima, jogo->tempoRestante, jogo->pontuacao, jogo->onda, jogo->ouro);
    desenhar_texto(26.0f, 88.0f, linha, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);

    renderizar_retangulo(26.0f, 108.0f, 180.0f, 10.0f, (Color){0.2f, 0.2f, 0.2f, 0.9f});
    renderizar_retangulo(26.0f, 108.0f, 180.0f * (jogo->jogador.vida / jogo->jogador.vidaMaxima), 10.0f, (Color){0.2f, 0.85f, 0.35f, 0.9f});

    if (jogo->jogador.possuiPontoPonto || jogo->jogador.possuiAnguloPonto)
    {
        char ammo[32];
        snprintf(ammo, sizeof(ammo), "Missiles: %d/%d", jogo->jogador.municaoGuiada, jogo->jogador.municaoGuiadaMaxima);
        desenhar_texto(220.0f, 118.0f, ammo, GLUT_BITMAP_HELVETICA_12, 1.0f, 0.6f, 0.2f);
    }

    if (jogo->temporizadorUltimaMelhoria > 0.0f)
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "Upgrade: %s", jogo->ultimaMelhoria);
        renderizar_retangulo(jogo->largura - 280.0f, 68.0f, 250.0f, 36.0f, (Color){0.1f, 0.2f, 0.35f, 0.6f});
        desenhar_texto(jogo->largura - 266.0f, 90.0f, msg, GLUT_BITMAP_HELVETICA_18, 0.9f, 0.98f, 1.0f);
    }

    if (jogo->temporizadorToast > 0.0f && jogo->mensagemToast[0] != '\0')
    {
        renderizar_retangulo(jogo->largura - 320.0f, 26.0f, 290.0f, 34.0f, (Color){0.06f, 0.12f, 0.25f, 0.72f});
        desenhar_texto(jogo->largura - 306.0f, 47.0f, jogo->mensagemToast, GLUT_BITMAP_HELVETICA_12, 0.92f, 0.98f, 1.0f);
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
    renderizar_circulo(m, 4.0f, (Color){0.3f, 0.9f, 1.0f, 0.65f}, 12);
}

void desenhar_icone_melhoria(TipoMelhoria tipo, float x, float y)
{
    switch (tipo)
    {
    case MELHORIA_DANO:
        renderizar_estrela(matematica_vetor2d(x, y), 16.0f, (Color){1.0f, 0.7f, 0.25f, 0.95f});
        break;
    case MELHORIA_TAXA_DE_DISPARO:
        renderizar_triangulo(matematica_vetor2d(x, y), 14.0f, (Color){1.0f, 0.95f, 0.5f, 0.95f});
        renderizar_triangulo(matematica_vetor2d(x + 10.0f, y), 10.0f, (Color){1.0f, 0.75f, 0.2f, 0.85f});
        break;
    case MELHORIA_VELOCIDADE:
        renderizar_losangulo(matematica_vetor2d(x - 6.0f, y), 10.0f, (Color){0.5f, 1.0f, 0.9f, 0.95f});
        renderizar_losangulo(matematica_vetor2d(x + 8.0f, y), 8.0f, (Color){0.3f, 0.9f, 1.0f, 0.9f});
        break;
    case MELHORIA_CURAR:
        renderizar_retangulo(x - 4.0f, y - 12.0f, 8.0f, 24.0f, (Color){0.45f, 1.0f, 0.45f, 0.95f});
        renderizar_retangulo(x - 12.0f, y - 4.0f, 24.0f, 8.0f, (Color){0.45f, 1.0f, 0.45f, 0.95f});
        break;
    case MELHORIA_TEMPO:
        renderizar_circulo(matematica_vetor2d(x, y), 13.0f, (Color){0.7f, 0.85f, 1.0f, 0.95f}, 16);
        renderizar_retangulo(x - 1.0f, y - 1.0f, 2.0f, 9.0f, (Color){0.1f, 0.2f, 0.5f, 0.95f});
        renderizar_retangulo(x - 1.0f, y - 1.0f, 7.0f, 2.0f, (Color){0.1f, 0.2f, 0.5f, 0.95f});
        break;
    case MELHORIA_ORIENTACAO_PONTO_PONTO:
        renderizar_circulo(matematica_vetor2d(x, y), 10.0f, (Color){0.2f, 0.9f, 1.0f, 0.9f}, 16);
        break;
    case MELHORIA_ORIENTACAO_ANGULO_PONTO:
        renderizar_circulo(matematica_vetor2d(x, y), 10.0f, (Color){0.9f, 0.2f, 1.0f, 0.9f}, 16);
        break;
    default:
        renderizar_circulo(matematica_vetor2d(x, y), 12.0f, (Color){1.0f, 1.0f, 1.0f, 0.8f}, 12);
        break;
    }
}

void desenhar_menu(Jogo *jogo)
{
    static float hoverAnim[3] = {0.0f, 0.0f, 0.0f};
    float centerX = jogo->largura * 0.5f;
    float uiScale = (float)jogo->largura / 1280.0f;
    float hScale = (float)jogo->altura / 720.0f;
    float btnW, startH, subH, shadowPad;
    float bx, by, obx, oby, lbx, lby;
    float mx = (float)jogo->entrada.mouseX;
    float my = (float)jogo->entrada.mouseY;
    float pulse = 0.5f + 0.5f * sinf(jogo->tempoDecorrido * 6.0f);
    int hoverStart, hoverOptions, hoverScores, compactUI, idx, s;

    if (hScale < uiScale)
        uiScale = hScale;
    if (uiScale < 0.75f)
        uiScale = 0.75f;
    if (uiScale > 1.15f)
        uiScale = 1.15f;
    compactUI = (jogo->largura < 1100 || jogo->altura < 680);

    btnW = 300.0f * uiScale;
    startH = 60.0f * uiScale;
    subH = 50.0f * uiScale;
    shadowPad = 10.0f * uiScale;

    bx = centerX - btnW * 0.5f;
    by = jogo->altura * 0.30f;
    obx = centerX - btnW * 0.5f;
    oby = jogo->altura * 0.40f;
    lbx = centerX - btnW * 0.5f;
    lby = jogo->altura * 0.50f;

    hoverStart = (mx >= bx && mx <= bx + btnW && my >= by && my <= by + startH);
    hoverOptions = (mx >= obx && mx <= obx + btnW && my >= oby && my <= oby + subH);
    hoverScores = (mx >= lbx && mx <= lbx + btnW && my >= lby && my <= lby + subH);

    {
        float targets[3] = {hoverStart ? 1.0f : 0.0f, hoverOptions ? 1.0f : 0.0f, hoverScores ? 1.0f : 0.0f};
        for (idx = 0; idx < 3; ++idx)
        {
            hoverAnim[idx] += (targets[idx] - hoverAnim[idx]) * (8.0f * jogo->tempoDelta);
            if (hoverAnim[idx] < 0.0f)
                hoverAnim[idx] = 0.0f;
            if (hoverAnim[idx] > 1.0f)
                hoverAnim[idx] = 1.0f;
        }
    }

    desenhar_texto(centerX - 160.0f, jogo->altura * 0.14f, "ORBIT SIEGE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.98f, 0.95f);
    desenhar_texto(centerX - 310.0f, jogo->altura * 0.20f, "A platformer action game with upgrades and survival mechanics", GLUT_BITMAP_HELVETICA_12, 0.75f, 0.82f, 0.95f);
    if (!compactUI)
        desenhar_texto(centerX - 138.0f, jogo->altura * 0.24f, "Press O for options or L for leaderboard", GLUT_BITMAP_HELVETICA_12, 0.72f, 0.80f, 0.92f);

    if (hoverAnim[0] > 0.01f)
        renderizar_retangulo(bx - shadowPad - 4.0f * uiScale, by - 9.0f * uiScale,
                             btnW + (shadowPad + 4.0f * uiScale) * 2.0f, startH + 18.0f * uiScale,
                             (Color){0.45f, 0.75f, 1.0f, (0.10f + 0.18f * pulse) * hoverAnim[0]});
    renderizar_retangulo(bx - shadowPad, by - 5.0f * uiScale, btnW + shadowPad * 2.0f, startH + 10.0f * uiScale,
                         (Color){0.08f + 0.12f * hoverAnim[0], 0.25f + 0.20f * hoverAnim[0], 0.55f + 0.35f * hoverAnim[0], 0.50f + 0.05f * hoverAnim[0]});
    renderizar_retangulo(bx, by, btnW, startH,
                         (Color){0.15f + 0.07f * hoverAnim[0], 0.45f + 0.13f * hoverAnim[0], 0.85f + 0.15f * hoverAnim[0], 0.95f + 0.03f * hoverAnim[0]});
    desenhar_texto(centerX - 52.0f * uiScale, by + 20.0f * uiScale, "START GAME", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    desenhar_texto(centerX - 89.0f * uiScale, by + 40.0f * uiScale, "Press ENTER or click", GLUT_BITMAP_HELVETICA_10, 0.75f, 0.85f, 1.0f);

    if (hoverAnim[1] > 0.01f)
        renderizar_retangulo(obx - shadowPad - 3.0f * uiScale, oby - 8.0f * uiScale,
                             btnW + (shadowPad + 3.0f * uiScale) * 2.0f, subH + 16.0f * uiScale,
                             (Color){0.78f, 0.52f, 1.0f, (0.08f + 0.16f * pulse) * hoverAnim[1]});
    renderizar_retangulo(obx - shadowPad, oby - 5.0f * uiScale, btnW + shadowPad * 2.0f, subH + 10.0f * uiScale,
                         (Color){0.26f + 0.15f * hoverAnim[1], 0.12f + 0.07f * hoverAnim[1], 0.36f + 0.24f * hoverAnim[1], 0.50f + 0.05f * hoverAnim[1]});
    renderizar_retangulo(obx, oby, btnW, subH,
                         (Color){0.36f + 0.12f * hoverAnim[1], 0.22f + 0.10f * hoverAnim[1], 0.62f + 0.20f * hoverAnim[1], 0.90f + 0.05f * hoverAnim[1]});
    desenhar_texto(centerX - 52.0f * uiScale, oby + 17.0f * uiScale, "OPTIONS (O)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (hoverAnim[2] > 0.01f)
        renderizar_retangulo(lbx - shadowPad - 3.0f * uiScale, lby - 8.0f * uiScale,
                             btnW + (shadowPad + 3.0f * uiScale) * 2.0f, subH + 16.0f * uiScale,
                             (Color){0.45f, 0.95f, 0.90f, (0.08f + 0.16f * pulse) * hoverAnim[2]});
    renderizar_retangulo(lbx - shadowPad, lby - 5.0f * uiScale, btnW + shadowPad * 2.0f, subH + 10.0f * uiScale,
                         (Color){0.08f + 0.08f * hoverAnim[2], 0.22f + 0.18f * hoverAnim[2], 0.30f + 0.20f * hoverAnim[2], 0.50f + 0.05f * hoverAnim[2]});
    renderizar_retangulo(lbx, lby, btnW, subH,
                         (Color){0.16f + 0.10f * hoverAnim[2], 0.36f + 0.16f * hoverAnim[2], 0.50f + 0.18f * hoverAnim[2], 0.90f + 0.05f * hoverAnim[2]});
    desenhar_texto(centerX - 77.0f * uiScale, lby + 17.0f * uiScale, "LEADERBOARD (L)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (hoverStart || hoverOptions || hoverScores)
        desenhar_texto(centerX - 56.0f * uiScale, jogo->altura * 0.57f, "Click to select", GLUT_BITMAP_HELVETICA_12, 0.9f, 0.96f, 1.0f);

    renderizar_retangulo(jogo->largura * 0.5f - 400.0f, jogo->altura * 0.90f, 800.0f, compactUI ? 40.0f : 60.0f, (Color){0.0f, 0.0f, 0.0f, 0.25f});
    desenhar_texto(jogo->largura * 0.5f - 380.0f, jogo->altura * 0.925f, "A/D or ARROWS: Move  |  SPACE or W: Jump  |  Mouse: Aim  |  LClick: Shoot", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);
    if (!compactUI)
        desenhar_texto(jogo->largura * 0.5f - 380.0f, jogo->altura * 0.95f, "P or ESC: Pause  |  F12: Screenshot  |  Inimigo types: Orange (Standard), Diamond (Sniper), Heavy (Tank)", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);

    renderizar_retangulo(30.0f, jogo->altura * 0.78f, 320.0f, 90.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
    {
        char stats[128];
        snprintf(stats, sizeof(stats), "High Score: %d", jogo->maiorPontuacao);
        desenhar_texto(50.0f, jogo->altura * 0.82f, stats, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.4f);
        snprintf(stats, sizeof(stats), "Max Wave: %d", jogo->maiorOndaAtingida);
        desenhar_texto(50.0f, jogo->altura * 0.86f, stats, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.95f, 0.5f);
    }

    renderizar_retangulo(jogo->largura - 350.0f, jogo->altura * 0.78f, 320.0f, 90.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
    {
        char cfg[128];
        snprintf(cfg, sizeof(cfg), "Difficulty: %s", inimigo_nome_dificuldade(jogo->dificuldade));
        desenhar_texto(jogo->largura - 330.0f, jogo->altura * 0.82f, cfg, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.88f, 0.3f);
        snprintf(cfg, sizeof(cfg), "Audio: %s", jogo->audioHabilitado ? "ON" : "OFF");
        desenhar_texto(jogo->largura - 330.0f, jogo->altura * 0.86f, cfg, GLUT_BITMAP_HELVETICA_18, 0.3f, jogo->audioHabilitado ? 0.9f : 0.5f, 0.8f);
    }

    renderizar_retangulo(jogo->largura * 0.5f - 280.0f, jogo->altura * 0.60f, 560.0f, 140.0f, (Color){0.05f, 0.08f, 0.15f, 0.7f});
    desenhar_texto(jogo->largura * 0.5f - 150.0f, jogo->altura * 0.63f, "TOP 5 SCORES", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.9f, 0.5f);
    for (s = 0; s < jogo->contadorTopPontuacoes && s < 5; ++s)
    {
        char row[128];
        snprintf(row, sizeof(row), "%d. %-18s  %7d  W%d", s + 1, jogo->topPontuacoes[s].nome, jogo->topPontuacoes[s].pontuacao, jogo->topPontuacoes[s].onda);
        desenhar_texto(jogo->largura * 0.5f - 260.0f, jogo->altura * 0.67f + s * 20.0f, row, GLUT_BITMAP_HELVETICA_10, 0.8f, 0.88f, 0.98f);
    }
}

void desenhar_placar(Jogo *jogo)
{
    static float scoreHover[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float centerX = jogo->largura * 0.5f;
    float mx = (float)jogo->entrada.mouseX;
    float my = (float)jogo->entrada.mouseY;
    float pulse = 0.5f + 0.5f * sinf(jogo->tempoDecorrido * 6.0f);
    int hoverPrev, hoverNext, hoverClear, hoverBack, hi;
    int pageCount = (jogo->contadorTodasPontuacoes + jogo->tamanhoPaginaPontuacao - 1) / jogo->tamanhoPaginaPontuacao;
    int startIndex = jogo->paginaPontuacao * jogo->tamanhoPaginaPontuacao;
    int endIndex = startIndex + jogo->tamanhoPaginaPontuacao;
    char pageText[64];

    hoverPrev = (mx >= centerX - 255.0f && mx <= centerX - 95.0f && my >= jogo->altura * 0.20f && my <= jogo->altura * 0.20f + 44.0f);
    hoverNext = (mx >= centerX + 95.0f && mx <= centerX + 255.0f && my >= jogo->altura * 0.20f && my <= jogo->altura * 0.20f + 44.0f);
    hoverClear = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= jogo->altura * 0.20f && my <= jogo->altura * 0.20f + 44.0f);
    hoverBack = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= jogo->altura * 0.11f && my <= jogo->altura * 0.11f + 44.0f);

    {
        float targets[4] = {hoverPrev ? 1.0f : 0.0f, hoverNext ? 1.0f : 0.0f, hoverClear ? 1.0f : 0.0f, hoverBack ? 1.0f : 0.0f};
        for (hi = 0; hi < 4; ++hi)
        {
            scoreHover[hi] += (targets[hi] - scoreHover[hi]) * (8.0f * jogo->tempoDelta);
            if (scoreHover[hi] < 0.0f)
                scoreHover[hi] = 0.0f;
            if (scoreHover[hi] > 1.0f)
                scoreHover[hi] = 1.0f;
        }
    }

    if (pageCount <= 0)
        pageCount = 1;
    if (endIndex > jogo->contadorTodasPontuacoes)
        endIndex = jogo->contadorTodasPontuacoes;

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.0f, 0.0f, 0.0f, 0.67f});
    desenhar_texto(jogo->largura * 0.5f - 78.0f, jogo->altura * 0.82f, "SCOREBOARD", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    renderizar_retangulo(jogo->largura * 0.5f - 300.0f, jogo->altura * 0.26f, 600.0f, 360.0f, (Color){0.06f, 0.09f, 0.17f, 0.88f});
    desenhar_texto(jogo->largura * 0.5f - 270.0f, jogo->altura * 0.58f, "#   NAME                    SCORE       WAVE", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

    if (jogo->contadorTodasPontuacoes == 0)
    {
        desenhar_texto(jogo->largura * 0.5f - 96.0f, jogo->altura * 0.45f, "No scores saved yet", GLUT_BITMAP_HELVETICA_18, 1.0f, 0.9f, 0.7f);
    }
    else
    {
        int i;
        for (i = startIndex; i < endIndex; ++i)
        {
            char row[128];
            int rowY = (int)(jogo->altura * 0.53f - (i - startIndex) * 28.0f);
            snprintf(row, sizeof(row), "%02d  %-22s %8d   %4d", i + 1, jogo->todasPontuacoes[i].nome, jogo->todasPontuacoes[i].pontuacao, jogo->todasPontuacoes[i].onda);
            desenhar_texto(jogo->largura * 0.5f - 270.0f, (float)rowY, row, GLUT_BITMAP_HELVETICA_18, 0.85f, 0.93f, 1.0f);
        }
    }

    snprintf(pageText, sizeof(pageText), "Page %d/%d", jogo->paginaPontuacao + 1, pageCount);
    desenhar_texto(jogo->largura * 0.5f - 40.0f, jogo->altura * 0.22f, pageText, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);

    if (scoreHover[0] > 0.01f)
        renderizar_retangulo(centerX - 258.0f, jogo->altura * 0.20f - 3.0f, 166.0f, 50.0f, (Color){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[0]});
    renderizar_retangulo(jogo->largura * 0.5f - 255.0f, jogo->altura * 0.20f, 160.0f, 44.0f,
                         (Color){0.20f + 0.08f * scoreHover[0], 0.30f + 0.10f * scoreHover[0], 0.52f + 0.16f * scoreHover[0], 0.90f + 0.05f * scoreHover[0]});
    desenhar_texto(jogo->largura * 0.5f - 205.0f, jogo->altura * 0.228f, "PREV", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (scoreHover[1] > 0.01f)
        renderizar_retangulo(centerX + 92.0f, jogo->altura * 0.20f - 3.0f, 166.0f, 50.0f, (Color){0.48f, 0.68f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[1]});
    renderizar_retangulo(jogo->largura * 0.5f + 95.0f, jogo->altura * 0.20f, 160.0f, 44.0f,
                         (Color){0.20f + 0.08f * scoreHover[1], 0.30f + 0.10f * scoreHover[1], 0.52f + 0.16f * scoreHover[1], 0.90f + 0.05f * scoreHover[1]});
    desenhar_texto(jogo->largura * 0.5f + 147.0f, jogo->altura * 0.228f, "NEXT", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (scoreHover[2] > 0.01f)
        renderizar_retangulo(centerX - 113.0f, jogo->altura * 0.20f - 3.0f, 226.0f, 50.0f, (Color){1.0f, 0.52f, 0.54f, (0.10f + 0.12f * pulse) * scoreHover[2]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.20f, 220.0f, 44.0f,
                         (Color){0.45f + 0.10f * scoreHover[2], 0.20f + 0.06f * scoreHover[2], 0.22f + 0.08f * scoreHover[2], 0.90f + 0.05f * scoreHover[2]});
    desenhar_texto(jogo->largura * 0.5f - 78.0f, jogo->altura * 0.228f, "CLEAR (C)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (scoreHover[3] > 0.01f)
        renderizar_retangulo(centerX - 113.0f, jogo->altura * 0.11f - 3.0f, 226.0f, 50.0f, (Color){0.52f, 0.78f, 1.0f, (0.10f + 0.12f * pulse) * scoreHover[3]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.11f, 220.0f, 44.0f,
                         (Color){0.10f + 0.08f * scoreHover[3], 0.35f + 0.12f * scoreHover[3], 0.70f + 0.14f * scoreHover[3], 0.90f + 0.05f * scoreHover[3]});
    desenhar_texto(jogo->largura * 0.5f - 82.0f, jogo->altura * 0.138f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void desenhar_opcoes(Jogo *jogo)
{
    static float optHover[3] = {0.0f, 0.0f, 0.0f};
    float centerX = jogo->largura * 0.5f;
    float mx = (float)jogo->entrada.mouseX;
    float my = (float)jogo->entrada.mouseY;
    float pulse = 0.5f + 0.5f * sinf(jogo->tempoDecorrido * 6.0f);
    int hoverAudio = (mx >= centerX - 180.0f && mx <= centerX + 180.0f && my >= jogo->altura * 0.56f && my <= jogo->altura * 0.56f + 56.0f);
    int hoverDiff = (mx >= centerX - 180.0f && mx <= centerX + 180.0f && my >= jogo->altura * 0.46f && my <= jogo->altura * 0.46f + 56.0f);
    int hoverBack = (mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= jogo->altura * 0.31f && my <= jogo->altura * 0.31f + 50.0f);
    int oi;
    char linha[128];

    {
        float targets[3] = {hoverAudio ? 1.0f : 0.0f, hoverDiff ? 1.0f : 0.0f, hoverBack ? 1.0f : 0.0f};
        for (oi = 0; oi < 3; ++oi)
        {
            optHover[oi] += (targets[oi] - optHover[oi]) * (8.0f * jogo->tempoDelta);
            if (optHover[oi] < 0.0f)
                optHover[oi] = 0.0f;
            if (optHover[oi] > 1.0f)
                optHover[oi] = 1.0f;
        }
    }

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.0f, 0.0f, 0.0f, 0.62f});
    desenhar_texto(jogo->largura * 0.5f - 76.0f, jogo->altura * 0.73f, "OPTIONS", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    if (optHover[0] > 0.01f)
        renderizar_retangulo(centerX - 183.0f, jogo->altura * 0.56f - 3.0f, 366.0f, 62.0f, (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * optHover[0]});
    renderizar_retangulo(jogo->largura * 0.5f - 180.0f, jogo->altura * 0.56f, 360.0f, 56.0f,
                         (Color){0.20f + 0.08f * optHover[0], 0.25f + 0.10f * optHover[0], 0.50f + 0.16f * optHover[0], 0.90f + 0.05f * optHover[0]});
    snprintf(linha, sizeof(linha), "Audio: %s  (click or A)", jogo->audioHabilitado ? "ON" : "OFF");
    desenhar_texto(jogo->largura * 0.5f - 150.0f, jogo->altura * 0.595f, linha, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    if (optHover[1] > 0.01f)
        renderizar_retangulo(centerX - 183.0f, jogo->altura * 0.46f - 3.0f, 366.0f, 62.0f, (Color){0.84f, 0.66f, 1.0f, (0.10f + 0.12f * pulse) * optHover[1]});
    renderizar_retangulo(jogo->largura * 0.5f - 180.0f, jogo->altura * 0.46f, 360.0f, 56.0f,
                         (Color){0.28f + 0.10f * optHover[1], 0.22f + 0.08f * optHover[1], 0.44f + 0.15f * optHover[1], 0.90f + 0.05f * optHover[1]});
    snprintf(linha, sizeof(linha), "Difficulty: %s  (click or D)", inimigo_nome_dificuldade(jogo->dificuldade));
    desenhar_texto(jogo->largura * 0.5f - 150.0f, jogo->altura * 0.495f, linha, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    desenhar_texto(jogo->largura * 0.5f - 180.0f, jogo->altura * 0.40f, "Easy: less damage + more time | Hard: stronger/faster enemies", GLUT_BITMAP_HELVETICA_12, 0.92f, 0.92f, 0.92f);

    if (optHover[2] > 0.01f)
        renderizar_retangulo(centerX - 113.0f, jogo->altura * 0.31f - 3.0f, 226.0f, 56.0f, (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * optHover[2]});
    renderizar_retangulo(jogo->largura * 0.5f - 110.0f, jogo->altura * 0.31f, 220.0f, 50.0f,
                         (Color){0.10f + 0.08f * optHover[2], 0.35f + 0.12f * optHover[2], 0.70f + 0.14f * optHover[2], 0.90f + 0.05f * optHover[2]});
    desenhar_texto(jogo->largura * 0.5f - 76.0f, jogo->altura * 0.342f, "BACK MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void desenhar_pausa(Jogo *jogo)
{
    float centerX = jogo->largura * 0.5f;
    int compactUI = (jogo->largura < 1100 || jogo->altura < 680);
    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.0f, 0.0f, 0.0f, 0.56f});
    renderizar_retangulo(centerX - 240.0f, jogo->altura * 0.42f, 480.0f, compactUI ? 110.0f : 140.0f, (Color){0.06f, 0.10f, 0.18f, 0.88f});
    desenhar_texto(centerX - 64.0f, jogo->altura * 0.47f, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    desenhar_texto(centerX - 150.0f, jogo->altura * 0.53f, "Press P or ENTER to continue", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
    if (!compactUI)
        desenhar_texto(centerX - 120.0f, jogo->altura * 0.58f, "ESC also resumes the game", GLUT_BITMAP_HELVETICA_12, 0.78f, 0.86f, 0.98f);
}

void desenhar_melhorias_tela(Jogo *jogo)
{
    static float upgradeHoverAnim[MAXIMO_OPCOES_UPGRADE] = {0.0f, 0.0f, 0.0f};
    static float rerollHoverAnim = 0.0f;
    Vetor2D mouse = matematica_vetor2d((float)jogo->entrada.mouseX, (float)jogo->entrada.mouseY);
    float centerX = jogo->largura * 0.5f;
    float uiScale = (float)jogo->largura / 1280.0f;
    float hScale = (float)jogo->altura / 720.0f;
    float cardW, cardH, cardGap, totalW, startX, cardY;
    float rerollW, rerollH, rerollX, rerollY;
    float pulse = 0.5f + 0.5f * sinf(jogo->tempoDecorrido * 6.0f);
    int compactUI, i;

    if (hScale < uiScale)
        uiScale = hScale;
    if (uiScale < 0.78f)
        uiScale = 0.78f;
    if (uiScale > 1.08f)
        uiScale = 1.08f;
    compactUI = (jogo->largura < 1100 || jogo->altura < 680);

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

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.0f, 0.0f, 0.0f, 0.58f});
    renderizar_retangulo(centerX - totalW * 0.5f - 40.0f * uiScale, cardY - 70.0f * uiScale,
                         totalW + 80.0f * uiScale, cardH + 190.0f * uiScale, (Color){0.07f, 0.10f, 0.18f, 0.86f});
    desenhar_texto(centerX - 92.0f, cardY - 34.0f * uiScale, "CHOOSE UPGRADE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
    if (!compactUI)
        desenhar_texto(centerX - 155.0f, cardY - 12.0f * uiScale, "Select with mouse click or keys 1, 2, 3", GLUT_BITMAP_HELVETICA_12, 0.80f, 0.88f, 1.0f);
    jogo->melhoriaSelecionada = -1;

    for (i = 0; i < MAXIMO_OPCOES_UPGRADE; ++i)
    {
        float bx = startX + i * (cardW + cardGap);
        float by = cardY;
        int hovered = (mouse.x >= bx && mouse.x <= bx + cardW && mouse.y >= by && mouse.y <= by + cardH);
        if (hovered)
            jogo->melhoriaSelecionada = i;
        upgradeHoverAnim[i] += ((hovered ? 1.0f : 0.0f) - upgradeHoverAnim[i]) * (8.0f * jogo->tempoDelta);
        if (upgradeHoverAnim[i] < 0.0f)
            upgradeHoverAnim[i] = 0.0f;
        if (upgradeHoverAnim[i] > 1.0f)
            upgradeHoverAnim[i] = 1.0f;

        if (upgradeHoverAnim[i] > 0.01f)
            renderizar_retangulo(bx - 4.0f * uiScale, by - 4.0f * uiScale, cardW + 8.0f * uiScale, cardH + 8.0f * uiScale,
                                 (Color){0.65f, 0.88f, 1.0f, (0.10f + 0.14f * pulse) * upgradeHoverAnim[i]});
        renderizar_retangulo(bx, by, cardW, cardH,
                             (Color){0.14f + 0.06f * upgradeHoverAnim[i], 0.20f + i * 0.07f + 0.06f * upgradeHoverAnim[i], 0.45f + 0.10f * upgradeHoverAnim[i], 0.92f + 0.04f * upgradeHoverAnim[i]});
        desenhar_icone_melhoria(jogo->melhorias[i].tipo, bx + cardW - 25.0f * uiScale, by + cardH - 26.0f * uiScale);
        desenhar_texto(bx + 12.0f * uiScale, by + 34.0f * uiScale, jogo->melhorias[i].rotulo, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        desenhar_texto_limites(bx + 12.0f * uiScale, by + 62.0f * uiScale, jogo->melhorias[i].descricao, GLUT_BITMAP_HELVETICA_12, 0.88f, 0.95f, 1.0f, cardW - 32.0f * uiScale);
        {
            char numIdx[8];
            snprintf(numIdx, sizeof(numIdx), "[%d]", i + 1);
            desenhar_texto(bx + 12.0f * uiScale, by + cardH - 14.0f * uiScale, numIdx, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.95f, 0.6f);
        }
    }

    {
        int rerollHovered = (mouse.x >= rerollX && mouse.x <= rerollX + rerollW && mouse.y >= rerollY && mouse.y <= rerollY + rerollH);
        rerollHoverAnim += ((rerollHovered ? 1.0f : 0.0f) - rerollHoverAnim) * (8.0f * jogo->tempoDelta);
        if (rerollHoverAnim < 0.0f)
            rerollHoverAnim = 0.0f;
        if (rerollHoverAnim > 1.0f)
            rerollHoverAnim = 1.0f;

        if (rerollHoverAnim > 0.01f)
            renderizar_retangulo(rerollX - 3.0f * uiScale, rerollY - 3.0f * uiScale, rerollW + 6.0f * uiScale, rerollH + 6.0f * uiScale,
                                 (Color){0.75f, 0.64f, 1.0f, (0.08f + 0.12f * pulse) * rerollHoverAnim});
        renderizar_retangulo(rerollX, rerollY, rerollW, rerollH,
                             (Color){0.22f + 0.10f * rerollHoverAnim, 0.18f + 0.08f * rerollHoverAnim, 0.35f + 0.12f * rerollHoverAnim, 0.92f + 0.04f * rerollHoverAnim});
        desenhar_texto(centerX - 90.0f * uiScale, rerollY + 26.0f * uiScale, "Reroll (R) - Cost: 3 gold", GLUT_BITMAP_HELVETICA_12, 1.0f, 0.95f, 0.9f);
    }

    if (jogo->melhoriaSelecionada >= 0 && jogo->melhoriaSelecionada < MAXIMO_OPCOES_UPGRADE)
    {
        renderizar_retangulo(centerX - 240.0f * uiScale, rerollY + rerollH + 12.0f * uiScale, 480.0f * uiScale, 40.0f * uiScale, (Color){0.08f, 0.12f, 0.24f, 0.85f});
        desenhar_texto(centerX - 220.0f * uiScale, rerollY + rerollH + 36.0f * uiScale, jogo->melhorias[jogo->melhoriaSelecionada].descricao, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.98f, 1.0f);
    }
}

void desenhar_fim(Jogo *jogo)
{
    static float endMenuHover = 0.0f;
    const char *titulo = (jogo->tela == TELA_VITORIA) ? "VICTORY" : "DEFEAT";
    float centerX = jogo->largura * 0.5f;
    float mx = (float)jogo->entrada.mouseX;
    float my = (float)jogo->entrada.mouseY;
    float pulse = 0.5f + 0.5f * sinf(jogo->tempoDecorrido * 6.0f);
    int hoverMenu = (jogo->nomeSalvo && mx >= centerX - 110.0f && mx <= centerX + 110.0f && my >= jogo->altura * 0.45f && my <= jogo->altura * 0.45f + 50.0f);
    char linha[160];

    endMenuHover += ((hoverMenu ? 1.0f : 0.0f) - endMenuHover) * (8.0f * jogo->tempoDelta);
    if (endMenuHover < 0.0f)
        endMenuHover = 0.0f;
    if (endMenuHover > 1.0f)
        endMenuHover = 1.0f;

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.0f, 0.0f, 0.0f, 0.62f});
    renderizar_retangulo(centerX - 280.0f, jogo->altura * 0.38f, 560.0f, 260.0f, (Color){0.08f, 0.10f, 0.18f, 0.86f});
    desenhar_texto(centerX - 60.0f, jogo->altura * 0.44f, titulo, GLUT_BITMAP_TIMES_ROMAN_24,
                   jogo->tela == TELA_VITORIA ? 0.9f : 1.0f,
                   jogo->tela == TELA_VITORIA ? 1.0f : 0.85f,
                   jogo->tela == TELA_VITORIA ? 0.9f : 0.85f);

    snprintf(linha, sizeof(linha), "Score: %d  |  Wave: %d", jogo->pontuacao, jogo->onda);
    desenhar_texto(centerX - 100.0f, jogo->altura * 0.50f, linha, GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 1.0f);

    if (!jogo->nomeSalvo)
    {
        desenhar_texto(centerX - 190.0f, jogo->altura * 0.57f, "Type name and press ENTER to save score:", GLUT_BITMAP_HELVETICA_18, 0.95f, 0.95f, 0.95f);
        renderizar_retangulo(centerX - 170.0f, jogo->altura * 0.61f, 340.0f, 40.0f, (Color){0.12f, 0.12f, 0.2f, 0.9f});
        desenhar_texto(centerX - 160.0f, jogo->altura * 0.635f, jogo->nomeJogador, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 0.8f);
    }
    else
    {
        desenhar_texto(centerX - 170.0f, jogo->altura * 0.57f, "Saved. Press ENTER or M to go menu.", GLUT_BITMAP_HELVETICA_18, 0.95f, 1.0f, 0.85f);
        if (endMenuHover > 0.01f)
            renderizar_retangulo(centerX - 113.0f, jogo->altura * 0.45f - 3.0f, 226.0f, 56.0f,
                                 (Color){0.52f, 0.80f, 1.0f, (0.10f + 0.12f * pulse) * endMenuHover});
        renderizar_retangulo(centerX - 110.0f, jogo->altura * 0.45f, 220.0f, 50.0f,
                             (Color){0.10f + 0.10f * endMenuHover, 0.35f + 0.12f * endMenuHover, 0.70f + 0.14f * endMenuHover, 0.90f + 0.05f * endMenuHover});
        desenhar_texto(centerX - 72.0f, jogo->altura * 0.482f, "MENU", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }
}

void desenhar_flash(Jogo *jogo)
{
    if (jogo->tela == TELA_JOGANDO && jogo->flashDano > 0.0f)
        renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.9f, 0.1f, 0.1f, 0.18f * jogo->flashDano});

    if (jogo->tela == TELA_JOGANDO && jogo->flashMelhoria > 0.0f)
        renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Color){0.2f, 0.8f, 1.0f, 0.12f * jogo->flashMelhoria});
}