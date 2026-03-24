#ifndef TYPES_H
#define TYPES_H

typedef struct Vec2 {
    float x;
    float y;
} Vec2;

typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color;

typedef enum GameScreen {
    SCREEN_MENU = 0,
    SCREEN_OPTIONS,
    SCREEN_SCORES,
    SCREEN_PLAYING,
    SCREEN_PAUSED,
    SCREEN_UPGRADE,
    SCREEN_WIN,
    SCREEN_LOSE
} GameScreen;

typedef enum UpgradeType {
    UPGRADE_DAMAGE = 0,
    UPGRADE_FIRE_RATE,
    UPGRADE_SPEED,
    UPGRADE_HEAL,
    UPGRADE_TIME,
    UPGRADE_COUNT
} UpgradeType;

typedef enum EnemyType {
    ENEMY_STANDARD = 0,
    ENEMY_SNIPER,
    ENEMY_TANK
} EnemyType;

#endif
