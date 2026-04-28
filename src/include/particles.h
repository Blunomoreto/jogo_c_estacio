#ifndef PARTICLES_H
#define PARTICLES_H

#include "game.h"

void particles_spawn(Game *g, Vec2 p, int count, Color color);
void particles_draw(Game *g);

#endif
