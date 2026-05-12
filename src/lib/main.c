#include "configuracao.h"
#include "entrada.h"
#include "jogo.h"
#include "pastas.h"

#include <GL/glut.h>

static Jogo g_game;

static void main_display_cb(void)
{
    jogo_renderizar(&g_game);
}

static void main_reshape_cb(int largura, int altura)
{
    if (altura <= 0)
    {
        altura = 1;
    }
    g_game.largura = largura;
    g_game.altura = altura;
    glViewport(0, 0, largura, altura);
}

static void main_timer_cb(int contador_frame)
{
    int tiques_atuais = glutGet(GLUT_ELAPSED_TIME);
    float delta_tempo = (float)(tiques_atuais - g_game.ultimos_ticks) / 1000.0f;
    if (delta_tempo < 0.0f)
    {
        delta_tempo = 0.0f;
    }
    if (delta_tempo > 0.033f)
    {
        delta_tempo = 0.033f;
    }

    g_game.ultimos_ticks = tiques_atuais;
    jogo_atualizar(&g_game, delta_tempo);
    entrada_iniciar_frame(&g_game);
    glutPostRedisplay();
    glutTimerFunc(ALVO_DELAY_FRAMES_MS, main_timer_cb, contador_frame + 1);
}

static void main_keyboard_down_cb(unsigned char tecla, int x, int y)
{
    entrada_tecla_pressionada(&g_game, tecla, x, y);
}

static void main_keyboard_up_cb(unsigned char tecla, int x, int y)
{
    entrada_tecla_levantada(&g_game, tecla, x, y);
}

static void main_special_down_cb(int tecla, int x, int y)
{
    entrada_especial_pressionado(&g_game, tecla, x, y);
}

static void main_special_up_cb(int tecla, int x, int y)
{
    entrada_especial_levantado(&g_game, tecla, x, y);
}

static void main_mouse_cb(int botao, int estado, int x, int y)
{
    entrada_mouse_pressionado(&g_game, botao, estado, x, y);
}

static void main_motion_cb(int x, int y)
{
    entrada_mouse_movido(&g_game, x, y);
}

int main(int argc, char **argv)
{
    pastas_criar();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(JANELA_LARGURA, JANELA_ALTURA);
    glutCreateWindow("Orbit Siege - C + GLUT");

    jogo_iniciar(&g_game, JANELA_LARGURA, JANELA_ALTURA);
    g_game.ultimos_ticks = glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(main_display_cb);
    glutReshapeFunc(main_reshape_cb);
    glutKeyboardFunc(main_keyboard_down_cb);
    glutKeyboardUpFunc(main_keyboard_up_cb);
    glutSpecialFunc(main_special_down_cb);
    glutSpecialUpFunc(main_special_up_cb);
    glutMouseFunc(main_mouse_cb);
    glutMotionFunc(main_motion_cb);
    glutPassiveMotionFunc(main_motion_cb);
    glutTimerFunc(ALVO_DELAY_FRAMES_MS, main_timer_cb, 0);

    glutMainLoop();
    return 0;
}
