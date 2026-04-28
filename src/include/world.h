#ifndef WORLD_H
#define WORLD_H

#include "game.h"

void world_clear_entities(Game *g);
void world_clear_obstacles(Game *g);
void world_setup_obstacles(Game *g);
void world_spawn_wave(Game *g);
void world_draw_obstacles(Game *g);
void world_draw_background(Game *g);

#endif
