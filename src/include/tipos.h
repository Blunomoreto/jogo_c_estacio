#ifndef TIPOS_H
#define TIPOS_H

#include "configuracao.h"

typedef struct Vetor2D
{
    float x;
    float y;
} Vetor2D;

typedef struct Color
{
    float r;
    float g;
    float b;
    float a;
} Color;

typedef enum GameScreen
{
    SCREEN_MENU = 0,
    SCREEN_OPTIONS,
    SCREEN_SCORES,
    SCREEN_PLAYING,
    SCREEN_PAUSED,
    SCREEN_UPGRADE,
    SCREEN_WIN,
    SCREEN_LOSE
} GameScreen;

typedef enum UpgradeType
{
    UPGRADE_DAMAGE = 0,
    UPGRADE_FIRE_RATE,
    UPGRADE_SPEED,
    UPGRADE_HEAL,
    UPGRADE_TIME,
    UPGRADE_GUIDANCE_PP,
    UPGRADE_GUIDANCE_APNG,
    UPGRADE_AMMO,
    UPGRADE_COUNT
} UpgradeType;

typedef enum EnemyType
{
    ENEMY_STANDARD = 0,
    ENEMY_SNIPER,
    ENEMY_TANK,
    ENEMY_DIAMOND,
    ENEMY_PENTAGON
} EnemyType;

typedef enum GuidanceType
{
    GUIDANCE_NONE = 0,
    GUIDANCE_PP,
    GUIDANCE_APNG
} GuidanceType;

typedef struct ScoreEntry
{
    char name[24];
    int score;
    int wave;
} ScoreEntry;

typedef struct InputState
{
    unsigned char keys[256];
    unsigned char keysPressed[256];
    unsigned char special[256];
    unsigned char mouseDown[3];
    unsigned char mousePressed[3];
    int mouseX;
    int mouseY;
} InputState;

typedef struct Player
{
    Vetor2D pos;
    float size;
    float hp;
    float maxHp;
    float speed;
    float damage;
    float fireRate;
    float fireCooldown;

    float projectileSpeed;
    float velY;
    int isOnGround;
    float jumpPressedTime;
    Vetor2D vel;
    Vetor2D accel;

    int hasPP;
    int hasAPNG;
    int guidedAmmo;
    int maxGuidedAmmo;
    float maxLatAccel;
} Player;

typedef struct Enemy
{
    int active;
    int isBoss;
    EnemyType type;
    Vetor2D center;
    float orbitRadius;
    float angle;
    float angularSpeed;
    float size;
    float hp;
    float maxHp;
    float damage;
    float shootCooldown;
    float hitFlash;

    int burstCount;
    float burstTimer;

    Vetor2D vel;
    Vetor2D accel;
} Enemy;

typedef struct Projectile
{
    int active;
    int fromPlayer;
    Vetor2D pos;
    Vetor2D vel;
    float radius;
    float life;
    float damage;

    GuidanceType guidance;
    int targetIdx;
    float actualLatAccel;
    float fuelTimer;
    float prevDist;
    int missed;
    float sdTimer;
    float maxLatAccel;
} Projectile;

typedef struct Particle
{
    int active;
    Vetor2D pos;
    Vetor2D vel;
    float size;
    float life;
    Color color;
} Particle;

typedef struct Obstacle
{
    int active;
    float x;
    float y;
    float w;
    float h;
} Obstacle;

typedef struct UpgradeOption
{
    UpgradeType type;
    char label[64];
    char desc[96];
} UpgradeOption;

typedef struct Game
{
    GameScreen screen;
    InputState input;

    int width;
    int height;

    int running;
    int lastTicks;
    float deltaTime;

    Player player;
    Enemy enemies[MAXIMO_INIMIGOS];
    Projectile projectiles[MAXIMO_PROJETEIS];
    Particle particles[MAXIMO_PARTICULAS];
    Obstacle obstacles[MAXIMO_PLATAFORMAS];

    int wave;
    int enemiesRemaining;
    int wavesToWin;

    float timeLeft;
    float elapsed;
    int score;
    int gold;
    float damageFlash;
    float upgradeFlash;
    int upgradeHover;
    char lastUpgrade[64];
    float lastUpgradeTimer;
    char toastMessage[128];
    float toastTimer;
    int audioEnabled;
    int difficulty;

    UpgradeOption upgrades[MAXIMO_OPCOES_UPGRADE];

    int highScore;
    int maxWaveEver;
    ScoreEntry topScores[5];
    int topScoreCount;
    ScoreEntry allScores[64];
    int allScoreCount;
    int scorePage;
    int scorePageSize;

    char playerName[24];
    int enteringName;
    int nameSaved;

    unsigned int bgTexture;
    int bgTextureLoaded;
} Game;

#endif
