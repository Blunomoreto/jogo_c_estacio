#include "enemy.h"
#include "maths.h"

#include <math.h>

Vec2 enemy_mouse_to_world(Game *g)
{
    return maths_vec2((float)g->input.mouseX, (float)g->input.mouseY);
}

Vec2 enemy_position(const Enemy *e)
{
    return maths_vec2(e->center.x + cosf(e->angle) * e->orbitRadius,
                      e->center.y + sinf(e->angle) * e->orbitRadius);
}

Vec2 enemy_velocity(const Enemy *e)
{
    return maths_vec2(-e->orbitRadius * e->angularSpeed * sinf(e->angle),
                      e->orbitRadius * e->angularSpeed * cosf(e->angle));
}

Vec2 enemy_acceleration(const Enemy *e)
{
    float omega = e->angularSpeed;
    return maths_vec2(-e->orbitRadius * omega * omega * cosf(e->angle),
                      -e->orbitRadius * omega * omega * sinf(e->angle));
}

const char *enemy_difficulty_name(int difficulty)
{
    if (difficulty <= 0)
        return "EASY";
    if (difficulty >= 2)
        return "HARD";
    return "NORMAL";
}

float enemy_hp_mul(int difficulty)
{
    if (difficulty <= 0)
        return DIFF_EASY_ENEMY_HP_MUL;
    if (difficulty >= 2)
        return DIFF_HARD_ENEMY_HP_MUL;
    return DIFF_NORMAL_ENEMY_HP_MUL;
}

float enemy_damage_mul(int difficulty)
{
    if (difficulty <= 0)
        return DIFF_EASY_ENEMY_DMG_MUL;
    if (difficulty >= 2)
        return DIFF_HARD_ENEMY_DMG_MUL;
    return DIFF_NORMAL_ENEMY_DMG_MUL;
}

float enemy_fire_rate_mul(int difficulty)
{
    if (difficulty <= 0)
        return DIFF_EASY_ENEMY_FIRE_MUL;
    if (difficulty >= 2)
        return DIFF_HARD_ENEMY_FIRE_MUL;
    return DIFF_NORMAL_ENEMY_FIRE_MUL;
}

float enemy_time_start(int difficulty)
{
    if (difficulty <= 0)
        return RUN_TIME_LIMIT + DIFF_EASY_TIME_BONUS;
    if (difficulty >= 2)
        return RUN_TIME_LIMIT + DIFF_HARD_TIME_BONUS;
    return RUN_TIME_LIMIT + DIFF_NORMAL_TIME_BONUS;
}

int enemy_waves_to_win(int difficulty)
{
    int v;
    if (difficulty <= 0)
        v = WAVES_TO_WIN + DIFF_EASY_WAVE_OFFSET;
    else if (difficulty >= 2)
        v = WAVES_TO_WIN + DIFF_HARD_WAVE_OFFSET;
    else
        v = WAVES_TO_WIN + DIFF_NORMAL_WAVE_OFFSET;
    if (v < 1)
        v = 1;
    return v;
}

float enemy_score_mul(int difficulty)
{
    if (difficulty <= 0)
        return DIFF_EASY_SCORE_MUL;
    if (difficulty >= 2)
        return DIFF_HARD_SCORE_MUL;
    return DIFF_NORMAL_SCORE_MUL;
}
