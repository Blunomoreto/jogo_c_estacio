#ifndef UPGRADES_H
#define UPGRADES_H

#include "game.h"

void upgrades_fill_option(UpgradeOption *o, UpgradeType type);
void upgrades_roll(Game *g);
void upgrades_apply(Game *g, UpgradeType t);
void upgrades_draw_icon(UpgradeType t, float x, float y);
void upgrades_choose(Game *g, int idx);

#endif
