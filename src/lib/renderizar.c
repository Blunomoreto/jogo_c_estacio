#include "renderizar.h"
#include "jogo.h"
#include "inimigo.h"
#include "matematica.h"

#include <GL/glut.h>
#include <math.h>

void renderizar_perspectiva_ortografica(struct Game *jogo)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)jogo->width, (GLdouble)jogo->height, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderizar_retangulo(float x, float y, float largura, float altura, Color cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + largura, y);
    glVertex2f(x + largura, y + altura);
    glVertex2f(x, y + altura);
    glEnd();
}

void renderizar_circulo(Vetor2D vetor, float raio, Color cor, int segmentos)
{
    int i;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (i = 0; i <= segmentos; ++i)
    {
        float a = (float)i / (float)segmentos * 2.0f * (float)M_PI;
        glVertex2f(vetor.x + cosf(a) * raio, vetor.y + sinf(a) * raio);
    }
    glEnd();
}

void renderizar_triangulo(Vetor2D vetor, float tamanho, Color cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLES);
    glVertex2f(vetor.x, vetor.y + tamanho);
    glVertex2f(vetor.x - tamanho * 0.8f, vetor.y - tamanho * 0.7f);
    glVertex2f(vetor.x + tamanho * 0.8f, vetor.y - tamanho * 0.7f);
    glEnd();
}

void renderizar_losangulo(Vetor2D vetor, float tamanho, Color cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_QUADS);
    glVertex2f(vetor.x, vetor.y + tamanho);
    glVertex2f(vetor.x + tamanho, vetor.y);
    glVertex2f(vetor.x, vetor.y - tamanho);
    glVertex2f(vetor.x - tamanho, vetor.y);
    glEnd();
}

void renderizar_estrela(Vetor2D vetor, float tamanho, Color cor)
{
    int i;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (i = 0; i <= 10; ++i)
    {
        float a = (float)i / 10.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float r = (i % 2 == 0) ? tamanho : tamanho * 0.45f;
        glVertex2f(vetor.x + cosf(a) * r, vetor.y + sinf(a) * r);
    }
    glEnd();
}

void renderizar_pentagono(Vetor2D vetor, float tamanho, Color cor)
{
    int i;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (i = 0; i <= 5; ++i)
    {
        float a = (float)i / 5.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        glVertex2f(vetor.x + cosf(a) * tamanho, vetor.y + sinf(a) * tamanho);
    }
    glEnd();
}

void renderizar_fundo(struct Game *jogo)
{
    float offset = fmodf(jogo->elapsed * 20.0f, (float)jogo->width);

    if (jogo->bgTextureLoaded)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, jogo->bgTexture);
        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + offset / jogo->width, 0.0f);
        glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + offset / jogo->width, 0.0f);
        glVertex2f((float)jogo->width, 0.0f);
        glTexCoord2f(1.0f + offset / jogo->width, 1.0f);
        glVertex2f((float)jogo->width, (float)jogo->height);
        glTexCoord2f(0.0f + offset / jogo->width, 1.0f);
        glVertex2f(0.0f, (float)jogo->height);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->width, (float)jogo->height, (Color){0.04f, 0.06f, 0.11f, 1.0f});

    renderizar_circulo(matematica_vetor2d(jogo->width * 0.85f, jogo->height * 0.25f), 60.0f, (Color){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    renderizar_circulo(matematica_vetor2d(jogo->width * 0.88f, jogo->height * 0.22f), 58.0f, (Color){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderizar_retangulo(0.0f, 0.0f, (float)jogo->width, (float)jogo->height, (Color){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int i;
        for (i = 0; i < 18; ++i)
        {
            float x = fmodf((i * 97.0f + jogo->elapsed * (8.0f + i)), (float)jogo->width);
            float y = 80.0f + fmodf((i * 61.0f + jogo->elapsed * (5.0f + i * 0.25f)), (float)jogo->height - 100.0f);
            renderizar_circulo(matematica_vetor2d(x, y), 2.0f + (i % 3), (Color){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    renderizar_retangulo(0.0f, ALTURA_CHAO, (float)jogo->width, (float)jogo->height - ALTURA_CHAO, (Color){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int i;
        for (i = 0; i < 20; ++i)
        {
            float x1 = i * (jogo->width / 20.0f);
            renderizar_retangulo(x1, ALTURA_CHAO - 4.0f, (jogo->width / 40.0f), 3.0f, (Color){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}

void renderizar_plataformas(struct Game *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PLATAFORMAS; ++i)
    {
        Obstacle *o = &jogo->obstacles[i];
        if (!o->active)
            continue;
        renderizar_retangulo(o->x, o->y, o->w, o->h, (Color){0.22f, 0.28f, 0.38f, 0.82f});
        renderizar_retangulo(o->x + 3.0f, o->y + 3.0f, o->w - 6.0f, o->h - 6.0f, (Color){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void renderizar_jogador(struct Game *jogo)
{
    Vetor2D vetor = jogo->player.pos;
    float tamanho = jogo->player.size;

    renderizar_circulo(matematica_vetor2d(vetor.x, vetor.y - tamanho * 0.4f), tamanho * 0.35f, (Color){0.95f, 0.85f, 0.70f, 1.0f}, 12);
    renderizar_circulo(matematica_vetor2d(vetor.x - tamanho * 0.12f, vetor.y - tamanho * 0.5f), tamanho * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_circulo(matematica_vetor2d(vetor.x + tamanho * 0.12f, vetor.y - tamanho * 0.5f), tamanho * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_retangulo(vetor.x - tamanho * 0.22f, vetor.y - tamanho * 0.08f, tamanho * 0.44f, tamanho * 0.35f, (Color){0.2f, 0.3f, 0.7f, 1.0f});
    renderizar_retangulo(vetor.x - tamanho * 0.35f, vetor.y - tamanho * 0.05f, tamanho * 0.25f, tamanho * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(vetor.x + tamanho * 0.1f, vetor.y - tamanho * 0.05f, tamanho * 0.25f, tamanho * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(vetor.x - tamanho * 0.12f, vetor.y + tamanho * 0.28f, tamanho * 0.15f, tamanho * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});
    renderizar_retangulo(vetor.x + tamanho * 0.07f, vetor.y + tamanho * 0.28f, tamanho * 0.15f, tamanho * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});

    if (!jogo->player.isOnGround)
        renderizar_circulo(vetor, tamanho * 0.9f, (Color){0.3f, 0.9f, 1.0f, 0.2f}, 16);
}

void renderizar_inimigo(struct Enemy *inimigo)
{
    Vetor2D vetor = inimigo_posicao(inimigo);
    if (inimigo->isBoss)
    {
        Color base = (Color){0.95f, 0.15f + inimigo->hitFlash * 0.45f, 0.85f, 1.0f};
        renderizar_circulo(vetor, inimigo->size, base, 24);
        renderizar_estrela(vetor, inimigo->size * 0.72f, (Color){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderizar_circulo(vetor, inimigo->size * 1.8f, (Color){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (inimigo->type == ENEMY_TANK)
    {
        renderizar_circulo(vetor, inimigo->size, (Color){0.98f, 0.30f + inimigo->hitFlash * 0.25f, 0.18f, 1.0f}, 20);
        renderizar_retangulo(vetor.x - inimigo->size * 0.55f, vetor.y - inimigo->size * 0.55f, inimigo->size * 1.1f, inimigo->size * 1.1f, (Color){1.0f, 0.76f, 0.18f, 0.75f});
        renderizar_circulo(vetor, inimigo->size * 0.35f, (Color){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (inimigo->type == ENEMY_SNIPER)
    {
        renderizar_losangulo(vetor, inimigo->size * 1.05f, (Color){1.0f, 0.62f + inimigo->hitFlash * 0.25f, 0.24f, 1.0f});
        renderizar_triangulo(matematica_vetor2d(vetor.x, vetor.y - inimigo->size * 0.1f), inimigo->size * 0.5f, (Color){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (inimigo->type == ENEMY_DIAMOND)
    {
        float s = inimigo->size * 0.7f;
        renderizar_circulo(vetor, inimigo->size, (Color){1.0f, 0.45f + inimigo->hitFlash * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(vetor.x, vetor.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        renderizar_retangulo(-s, -s, s * 2.0f, s * 2.0f, (Color){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        renderizar_circulo(vetor, inimigo->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (inimigo->type == ENEMY_PENTAGON)
    {
        renderizar_pentagono(vetor, inimigo->size, (Color){0.2f, 0.9f, 1.0f, 1.0f});
        renderizar_circulo(vetor, inimigo->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        renderizar_circulo(vetor, inimigo->size, (Color){1.0f, 0.35f + inimigo->hitFlash * 0.4f, 0.20f + inimigo->hitFlash * 0.4f, 1.0f}, 20);
        renderizar_estrela(vetor, inimigo->size * 0.55f, (Color){1.0f, 0.95f, 0.35f, 0.9f});
    }
    renderizar_circulo(inimigo->center, 2.0f, (Color){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void renderizar_projeteis(struct Game *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *vetor = &jogo->projectiles[i];
        if (!vetor->active)
            continue;

        if (vetor->guidance != GUIDANCE_NONE)
        {
            float angle = atan2f(vetor->vel.y, vetor->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Color cor;
            if (vetor->missed)
                cor = (Color){1.0f, 0.0f, 0.0f, 1.0f};
            else if (vetor->guidance == GUIDANCE_APNG)
                cor = (Color){1.0f, 0.5f, 0.0f, 1.0f};
            else
                cor = (Color){0.8f, 0.0f, 1.0f, 1.0f};

            glPushMatrix();
            glTranslatef(vetor->pos.x, vetor->pos.y, 0.0f);
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            renderizar_triangulo(matematica_vetor2d(0, 0), vetor->radius * 1.4f, cor);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(vetor->pos, vetor->radius * 2.5f, (Color){cor.r, cor.g, cor.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Color corProjetil = vetor->fromPlayer ? (Color){0.3f, 0.95f, 1.0f, 0.9f} : (Color){1.0f, 0.8f, 0.0f, 0.9f};
            renderizar_circulo(vetor->pos, vetor->radius, corProjetil, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Color corHalo = vetor->fromPlayer ? (Color){0.3f, 0.85f, 1.0f, 0.28f} : (Color){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(vetor->pos, vetor->radius * 2.2f, corHalo, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void renderizar_particulas(struct Game *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &jogo->particles[i];
        if (!pt->active)
            continue;
        renderizar_circulo(pt->pos, pt->size, (Color){pt->color.r, pt->color.g, pt->color.b, pt->color.a * (pt->life * 2.0f)}, 8);
    }
}
