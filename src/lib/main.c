#include "config.h"
#include "game.h"

#include <GL/glut.h>

static Game g_game;

static void display_cb(void) {
    game_render(&g_game);
}

static void reshape_cb(int w, int h) {
    if (h <= 0) {
        h = 1;
    }
    g_game.width = w;
    g_game.height = h;
    glViewport(0, 0, w, h);
}

static void timer_cb(int value) {
    int ticks = glutGet(GLUT_ELAPSED_TIME);
    float dt = (float)(ticks - g_game.lastTicks) / 1000.0f;
    if (dt < 0.0f) {
        dt = 0.0f;
    }
    if (dt > 0.033f) {
        dt = 0.033f;
    }

    g_game.lastTicks = ticks;
    game_update(&g_game, dt);
    game_begin_frame(&g_game);
    glutPostRedisplay();
    glutTimerFunc(TARGET_FRAME_MS, timer_cb, value + 1);
}

static void keyboard_down_cb(unsigned char key, int x, int y) {
    game_on_key_down(&g_game, key, x, y);
}

static void keyboard_up_cb(unsigned char key, int x, int y) {
    game_on_key_up(&g_game, key, x, y);
}

static void special_down_cb(int key, int x, int y) {
    game_on_special_down(&g_game, key, x, y);
}

static void special_up_cb(int key, int x, int y) {
    game_on_special_up(&g_game, key, x, y);
}

static void mouse_cb(int button, int state, int x, int y) {
    game_on_mouse(&g_game, button, state, x, y);
}

static void motion_cb(int x, int y) {
    game_on_mouse_move(&g_game, x, y);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Orbit Siege - C + GLUT");

    game_init(&g_game, WINDOW_WIDTH, WINDOW_HEIGHT);
    g_game.lastTicks = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(display_cb);
    glutReshapeFunc(reshape_cb);
    glutKeyboardFunc(keyboard_down_cb);
    glutKeyboardUpFunc(keyboard_up_cb);
    glutSpecialFunc(special_down_cb);
    glutSpecialUpFunc(special_up_cb);
    glutMouseFunc(mouse_cb);
    glutMotionFunc(motion_cb);
    glutPassiveMotionFunc(motion_cb);
    glutTimerFunc(TARGET_FRAME_MS, timer_cb, 0);

    glutMainLoop();
    return 0;
}
