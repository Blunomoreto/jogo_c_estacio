#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"

Vec2 enemy_mouse_to_world(Game *g);
Vec2 enemy_position(const Enemy *e);
Vec2 enemy_velocity(const Enemy *e);
Vec2 enemy_acceleration(const Enemy *e);

const char *enemy_difficulty_name(int difficulty);

float enemy_hp_mul(int difficulty);
float enemy_damage_mul(int difficulty);
float enemy_fire_rate_mul(int difficulty);
float enemy_time_start(int difficulty);
float enemy_score_mul(int difficulty);

int enemy_waves_to_win(int difficulty);

#endif
