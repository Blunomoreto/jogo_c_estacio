#ifndef UI_H
#define UI_H

#include "game.h"

void ui_draw_player(Game *g);
void ui_draw_enemy(Enemy *e);
void ui_draw_boss_hp_bar(Game *g);
void ui_draw_hud(Game *g);
void ui_draw_crosshair(Game *g);
void ui_refresh_top_scores(Game *g);
void ui_refresh_all_scores(Game *g);
void ui_set_toast(Game *g, const char *msg);
void ui_set_end_screen(Game *g, GameScreen end);

#endif
