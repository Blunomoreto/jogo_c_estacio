#!/usr/bin/env python3
import re

# Ler arquivo
with open(r'c:\Users\Aluno\Documents\brunomoreto\src\game.c', 'r', encoding='utf-8') as f:
    content = f.read()

# Novo menu code
new_menu = '''    if (g->screen == SCREEN_MENU) {
        /* Button positions */
        float bx = g->width * 0.5f - 150.0f;  /* START button */
        float by = g->height * 0.48f;
        float obx = g->width * 0.5f - 150.0f; /* OPTIONS button */
        float oby = g->height * 0.38f;
        float lbx = g->width * 0.5f - 150.0f; /* SCOREBOARD button */
        float lby = g->height * 0.28f;
        int s;

        /* Title section */
        draw_text(g->width * 0.5f - 160.0f, g->height * 0.72f, "ORBIT SIEGE", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.98f, 0.95f);
        draw_text(g->width * 0.5f - 310.0f, g->height * 0.64f, "A platformer action game with upgrades and survival mechanics", GLUT_BITMAP_HELVETICA_12, 0.75f, 0.82f, 0.95f);

        /* Main buttons section with better spacing */
        /* START Button */
        draw_rect(bx - 10.0f, by - 5.0f, 320.0f, 70.0f, (Color){0.08f, 0.25f, 0.55f, 0.5f});  /* Shadow/glow */
        draw_rect(bx, by, 300.0f, 60.0f, (Color){0.15f, 0.45f, 0.85f, 0.95f});
        draw_text(bx + 110.0f, by + 20.0f, "START GAME", GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);
        draw_text(bx + 75.0f, by + 40.0f, "Press ENTER or click to begin", GLUT_BITMAP_HELVETICA_10, 0.75f, 0.85f, 1.0f);

        /* OPTIONS Button */
        draw_rect(obx - 10.0f, oby - 5.0f, 320.0f, 60.0f, (Color){0.1f, 0.15f, 0.35f, 0.5f});
        draw_rect(obx, oby, 300.0f, 50.0f, (Color){0.2f, 0.3f, 0.6f, 0.9f});
        draw_text(obx + 100.0f, oby + 17.0f, "OPTIONS (O)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        /* SCOREBOARD Button */
        draw_rect(lbx - 10.0f, lby - 5.0f, 320.0f, 60.0f, (Color){0.1f, 0.15f, 0.35f, 0.5f});
        draw_rect(lbx, lby, 300.0f, 50.0f, (Color){0.18f, 0.28f, 0.55f, 0.9f});
        draw_text(lbx + 70.0f, lby + 17.0f, "LEADERBOARD (L)", GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

        /* Controls section */
        draw_rect(g->width * 0.5f - 400.0f, g->height * 0.16f, 800.0f, 100.0f, (Color){0.0f, 0.0f, 0.0f, 0.25f});
        draw_text(g->width * 0.5f - 380.0f, g->height * 0.22f, "CONTROLS", GLUT_BITMAP_HELVETICA_14, 0.95f, 0.95f, 0.95f);
        draw_text(g->width * 0.5f - 380.0f, g->height * 0.19f, "A/D or ARROWS: Move  |  SPACE or W: Jump  |  Mouse: Aim  |  LClick: Shoot", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);
        draw_text(g->width * 0.5f - 380.0f, g->height * 0.165f, "P or ESC: Pause  |  F12: Screenshot  |  Enemy types: Orange (Standard), Diamond (Sniper), Heavy (Tank)", GLUT_BITMAP_HELVETICA_10, 0.7f, 0.78f, 0.88f);

        /* Status section - left side */
        draw_rect(30.0f, g->height * 0.04f, 320.0f, 100.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
        {
            char stats[128];
            snprintf(stats, sizeof(stats), "High Score: %d", g->highScore);
            draw_text(50.0f, g->height * 0.10f, stats, GLUT_BITMAP_HELVETICA_14, 1.0f, 0.9f, 0.4f);
            snprintf(stats, sizeof(stats), "Max Wave: %d", g->maxWaveEver);
            draw_text(50.0f, g->height * 0.06f, stats, GLUT_BITMAP_HELVETICA_14, 0.85f, 0.95f, 0.5f);
        }

        /* Settings section - right side */
        draw_rect(g->width - 350.0f, g->height * 0.04f, 320.0f, 100.0f, (Color){0.0f, 0.0f, 0.0f, 0.4f});
        {
            char cfg[128];
            snprintf(cfg, sizeof(cfg), "Difficulty: %s", difficulty_name(g->difficulty));
            draw_text(g->width - 330.0f, g->height * 0.10f, cfg, GLUT_BITMAP_HELVETICA_14, 1.0f, 0.88f, 0.3f);
            snprintf(cfg, sizeof(cfg), "Audio: %s", g->audioEnabled ? "ON" : "OFF");
            draw_text(g->width - 330.0f, g->height * 0.06f, cfg, GLUT_BITMAP_HELVETICA_14, 0.3f, g->audioEnabled ? 0.9f : 0.5f, 0.8f);
        }

        /* Top scores display - centered at bottom */
        draw_rect(g->width * 0.5f - 280.0f, g->height * 0.04f + 110.0f, 560.0f, 150.0f, (Color){0.05f, 0.08f, 0.15f, 0.7f});
        draw_text(g->width * 0.5f - 150.0f, g->height * 0.04f + 235.0f, "TOP 5 SCORES", GLUT_BITMAP_HELVETICA_14, 0.95f, 0.9f, 0.5f);
        for (s = 0; s < g->topScoreCount && s < 5; ++s) {
            char row[128];
            snprintf(row, sizeof(row), "%d. %-18s  %7d  W%d", s + 1, g->topScores[s].name, g->topScores[s].score, g->topScores[s].wave);
            draw_text(g->width * 0.5f - 260.0f, g->height * 0.04f + 210.0f - s * 22.0f, row, GLUT_BITMAP_HELVETICA_11, 0.8f, 0.88f, 0.98f);
        }
    }'''

# Pattern para encontrar o bloco antigo (versão simplificada)
pattern = r'    if \(g->screen == SCREEN_MENU\) \{[\s\S]*?for \(s = 0; s < g->topScoreCount; \+\+s\)[\s\S]*?\}\s*\}'

# Substituir
match = re.search(pattern, content)
if match:
    content = content[:match.start()] + new_menu + content[match.end():]
    # Escrever
    with open(r'c:\Users\Aluno\Documents\brunomoreto\src\game.c', 'w', encoding='utf-8') as f:
        f.write(content)
    print('✓ Menu UI revisado e atualizado com sucesso!')
else:
    print('✗ Padrão não encontrado no arquivo. Verificando estrutura...')
    # Tenta uma busca mais simples
    if 'if (g->screen == SCREEN_MENU)' in content:
        print('✓ Menu encontrado. Tentando estratégia alternativa...')
    else:
        print('✗ Menu não encontrado no arquivo!')
