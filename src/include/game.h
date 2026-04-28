#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "persistence.h"
#include "types.h"

void game_init(Game *g, int width, int height);
void game_update(Game *g, float dt);
void game_render(Game *g);
void game_restart(Game *g);
void game_on_key_down(Game *g, unsigned char key, int x, int y);
void game_on_key_up(Game *g, unsigned char key, int x, int y);
void game_on_special_down(Game *g, int key, int x, int y);
void game_on_special_up(Game *g, int key, int x, int y);
void game_on_mouse(Game *g, int button, int state, int x, int y);
void game_on_mouse_move(Game *g, int x, int y);
void game_begin_frame(Game *g);

#endif
