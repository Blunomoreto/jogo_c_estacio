#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "game.h"

void projectiles_spawn(Game *g, Vec2 pos, Vec2 dir, int fromPlayer, float speed, float damage, float radius, float life, GuidanceType guidance, int targetIdx, float maxLatAccel);
void projectiles_draw(Game *g);

#endif
