#include "config.h"
#include "game.h"
#include "folders.h"

#include <GL/glut.h>

static Game g_game;

static void main_display_cb(void)
{
    game_render(&g_game);
}

static void main_reshape_cb(int w, int h)
{
    if (h <= 0)
    {
        h = 1;
    }
    g_game.width = w;
    g_game.height = h;
    glViewport(0, 0, w, h);
}

static void main_timer_cb(int value)
{
    int ticks = glutGet(GLUT_ELAPSED_TIME);
    float dt = (float)(ticks - g_game.lastTicks) / 1000.0f;
    if (dt < 0.0f)
    {
        dt = 0.0f;
    }
    if (dt > 0.033f)
    {
        dt = 0.033f;
    }

    g_game.lastTicks = ticks;
    game_update(&g_game, dt);
    game_begin_frame(&g_game);
    glutPostRedisplay();
    glutTimerFunc(TARGET_FRAME_MS, main_timer_cb, value + 1);
}

static void main_keyboard_down_cb(unsigned char key, int x, int y)
{
    game_on_key_down(&g_game, key, x, y);
}

static void main_keyboard_up_cb(unsigned char key, int x, int y)
{
    game_on_key_up(&g_game, key, x, y);
}

static void main_special_down_cb(int key, int x, int y)
{
    game_on_special_down(&g_game, key, x, y);
}

static void main_special_up_cb(int key, int x, int y)
{
    game_on_special_up(&g_game, key, x, y);
}

static void main_mouse_cb(int button, int state, int x, int y)
{
    game_on_mouse(&g_game, button, state, x, y);
}

static void main_motion_cb(int x, int y)
{
    game_on_mouse_move(&g_game, x, y);
}

int main(int argc, char **argv)
{
    folders_setup();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Orbit Siege - C + GLUT");

    game_init(&g_game, WINDOW_WIDTH, WINDOW_HEIGHT);
    g_game.lastTicks = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(main_display_cb);
    glutReshapeFunc(main_reshape_cb);
    glutKeyboardFunc(main_keyboard_down_cb);
    glutKeyboardUpFunc(main_keyboard_up_cb);
    glutSpecialFunc(main_special_down_cb);
    glutSpecialUpFunc(main_special_up_cb);
    glutMouseFunc(main_mouse_cb);
    glutMotionFunc(main_motion_cb);
    glutPassiveMotionFunc(main_motion_cb);
    glutTimerFunc(TARGET_FRAME_MS, main_timer_cb, 0);

    glutMainLoop();
    return 0;
}
