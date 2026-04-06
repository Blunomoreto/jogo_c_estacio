#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "persistence.h"
#include "types.h"

typedef struct InputState {
    unsigned char keys[256];
    unsigned char keysPressed[256];
    unsigned char special[256];
    unsigned char mouseDown[3];
    unsigned char mousePressed[3];
    int mouseX;
    int mouseY;
} InputState;

typedef struct Player {
    Vec2 pos;
    float size;
    float hp;
    float maxHp;
    float speed;
    float damage;
    float fireRate;
    float fireCooldown;
    float projectileSpeed;
    /* Platformer physics */
    float velY;              /* Vertical velocity (positive = falling) */
    int isOnGround;          /* 1 if player is touching ground */
    float jumpPressedTime;   /* Time space/W has been held (for variable jump) */
    
    /* Guidance Upgrades */
    int hasPP;
    int hasAPNG;
    int guidedAmmo;
    int maxGuidedAmmo;
} Player;

typedef struct Enemy {
    int active;
    int isBoss;
    EnemyType type;
    Vec2 center;
    float orbitRadius;
    float angle;
    float angularSpeed;
    float size;
    float hp;
    float maxHp;
    float damage;
    float shootCooldown;
    float hitFlash;

    /* Diamond Burst Behavior */
    int burstCount;
    float burstTimer;
} Enemy;

typedef struct Projectile {
    int active;
    int fromPlayer;
    Vec2 pos;
    Vec2 vel;
    float radius;
    float life;
    float damage;

    /* Guidance Logic */
    GuidanceType guidance;
    int targetIdx;
    float actualLatAccel;
    float fuelTimer;
    float prevDist;
    int missed;
    float sdTimer;
    float maxLatAccel; /* Dynamic limit for enemies */
} Projectile;

typedef struct Particle {
    int active;
    Vec2 pos;
    Vec2 vel;
    float size;
    float life;
    Color color;
} Particle;

typedef struct Obstacle {
    int active;
    float x;
    float y;
    float w;
    float h;
} Obstacle;

typedef struct UpgradeOption {
    UpgradeType type;
    char label[64];
    char desc[96];
} UpgradeOption;

typedef struct Game {
    GameScreen screen;
    InputState input;

    int width;
    int height;

    int running;
    int lastTicks;
    float deltaTime;

    Player player;
    Enemy enemies[MAX_ENEMIES];
    Projectile projectiles[MAX_PROJECTILES];
    Particle particles[MAX_PARTICLES];
    Obstacle obstacles[MAX_OBSTACLES];

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

    UpgradeOption upgrades[MAX_UPGRADE_OPTIONS];

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

void game_init(Game* g, int width, int height);
void game_update(Game* g, float dt);
void game_render(Game* g);
void game_restart(Game* g);

void game_on_key_down(Game* g, unsigned char key, int x, int y);
void game_on_key_up(Game* g, unsigned char key, int x, int y);
void game_on_special_down(Game* g, int key, int x, int y);
void game_on_special_up(Game* g, int key, int x, int y);
void game_on_mouse(Game* g, int button, int state, int x, int y);
void game_on_mouse_move(Game* g, int x, int y);

void game_begin_frame(Game* g);

#endif

