#include "renderizar.h"
#include "jogo.h"
#include "inimigo.h"
#include "matematica.h"

#include <GL/glut.h>
#include <math.h>

void renderizar_perspectiva_ortografica(struct Game *g)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)g->width, (GLdouble)g->height, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderizar_retangulo(float x, float y, float w, float h, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void renderizar_circulo(Vetor2D p, float radius, Color c, int segments)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= segments; ++i)
    {
        float a = (float)i / (float)segments * 2.0f * (float)M_PI;
        glVertex2f(p.x + cosf(a) * radius, p.y + sinf(a) * radius);
    }
    glEnd();
}

void renderizar_triangulo(Vetor2D p, float size, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.x, p.y + size);
    glVertex2f(p.x - size * 0.8f, p.y - size * 0.7f);
    glVertex2f(p.x + size * 0.8f, p.y - size * 0.7f);
    glEnd();
}

void renderizar_losangulo(Vetor2D p, float size, Color c)
{
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_QUADS);
    glVertex2f(p.x, p.y + size);
    glVertex2f(p.x + size, p.y);
    glVertex2f(p.x, p.y - size);
    glVertex2f(p.x - size, p.y);
    glEnd();
}

void renderizar_estrela(Vetor2D p, float size, Color c)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= 10; ++i)
    {
        float a = (float)i / 10.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float r = (i % 2 == 0) ? size : size * 0.45f;
        glVertex2f(p.x + cosf(a) * r, p.y + sinf(a) * r);
    }
    glEnd();
}

void renderizar_pentagono(Vetor2D p, float size, Color c)
{
    int i;
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(p.x, p.y);
    for (i = 0; i <= 5; ++i)
    {
        float a = (float)i / 5.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        glVertex2f(p.x + cosf(a) * size, p.y + sinf(a) * size);
    }
    glEnd();
}

void renderizar_fundo(struct Game *g)
{
    float offset = fmodf(g->elapsed * 20.0f, (float)g->width);

    if (g->bgTextureLoaded)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g->bgTexture);
        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + offset / g->width, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + offset / g->width, 0.0f); glVertex2f((float)g->width, 0.0f);
        glTexCoord2f(1.0f + offset / g->width, 1.0f); glVertex2f((float)g->width, (float)g->height);
        glTexCoord2f(0.0f + offset / g->width, 1.0f); glVertex2f(0.0f, (float)g->height);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.04f, 0.06f, 0.11f, 1.0f});

    renderizar_circulo(matematica_vetor2d(g->width * 0.85f, g->height * 0.25f), 60.0f, (Color){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    renderizar_circulo(matematica_vetor2d(g->width * 0.88f, g->height * 0.22f), 58.0f, (Color){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderizar_retangulo(0.0f, 0.0f, (float)g->width, (float)g->height, (Color){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int i;
        for (i = 0; i < 18; ++i)
        {
            float x = fmodf((i * 97.0f + g->elapsed * (8.0f + i)), (float)g->width);
            float y = 80.0f + fmodf((i * 61.0f + g->elapsed * (5.0f + i * 0.25f)), (float)g->height - 100.0f);
            renderizar_circulo(matematica_vetor2d(x, y), 2.0f + (i % 3), (Color){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    renderizar_retangulo(0.0f, ALTURA_CHAO, (float)g->width, (float)g->height - ALTURA_CHAO, (Color){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int i;
        for (i = 0; i < 20; ++i)
        {
            float x1 = i * (g->width / 20.0f);
            renderizar_retangulo(x1, ALTURA_CHAO - 4.0f, (g->width / 40.0f), 3.0f, (Color){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}

void renderizar_plataformas(struct Game *g)
{
    int i;
    for (i = 0; i < MAXIMO_PLATAFORMAS; ++i)
    {
        Obstacle *o = &g->obstacles[i];
        if (!o->active) continue;
        renderizar_retangulo(o->x, o->y, o->w, o->h, (Color){0.22f, 0.28f, 0.38f, 0.82f});
        renderizar_retangulo(o->x + 3.0f, o->y + 3.0f, o->w - 6.0f, o->h - 6.0f, (Color){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void renderizar_jogador(struct Game *g)
{
    Vetor2D p = g->player.pos;
    float size = g->player.size;

    renderizar_circulo(matematica_vetor2d(p.x, p.y - size * 0.4f), size * 0.35f, (Color){0.95f, 0.85f, 0.70f, 1.0f}, 12);
    renderizar_circulo(matematica_vetor2d(p.x - size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_circulo(matematica_vetor2d(p.x + size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_retangulo(p.x - size * 0.22f, p.y - size * 0.08f, size * 0.44f, size * 0.35f, (Color){0.2f, 0.3f, 0.7f, 1.0f});
    renderizar_retangulo(p.x - size * 0.35f, p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(p.x + size * 0.1f,  p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(p.x - size * 0.12f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});
    renderizar_retangulo(p.x + size * 0.07f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});

    if (!g->player.isOnGround)
        renderizar_circulo(p, size * 0.9f, (Color){0.3f, 0.9f, 1.0f, 0.2f}, 16);
}

void renderizar_inimigo(struct Enemy *e)
{
    Vetor2D p = inimigo_posicao(e);
    if (e->isBoss)
    {
        Color base = (Color){0.95f, 0.15f + e->hitFlash * 0.45f, 0.85f, 1.0f};
        renderizar_circulo(p, e->size, base, 24);
        renderizar_estrela(p, e->size * 0.72f, (Color){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderizar_circulo(p, e->size * 1.8f, (Color){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (e->type == ENEMY_TANK)
    {
        renderizar_circulo(p, e->size, (Color){0.98f, 0.30f + e->hitFlash * 0.25f, 0.18f, 1.0f}, 20);
        renderizar_retangulo(p.x - e->size * 0.55f, p.y - e->size * 0.55f, e->size * 1.1f, e->size * 1.1f, (Color){1.0f, 0.76f, 0.18f, 0.75f});
        renderizar_circulo(p, e->size * 0.35f, (Color){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (e->type == ENEMY_SNIPER)
    {
        renderizar_losangulo(p, e->size * 1.05f, (Color){1.0f, 0.62f + e->hitFlash * 0.25f, 0.24f, 1.0f});
        renderizar_triangulo(matematica_vetor2d(p.x, p.y - e->size * 0.1f), e->size * 0.5f, (Color){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (e->type == ENEMY_DIAMOND)
    {
        float s = e->size * 0.7f;
        renderizar_circulo(p, e->size, (Color){1.0f, 0.45f + e->hitFlash * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        renderizar_retangulo(-s, -s, s * 2.0f, s * 2.0f, (Color){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        renderizar_circulo(p, e->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (e->type == ENEMY_PENTAGON)
    {
        renderizar_pentagono(p, e->size, (Color){0.2f, 0.9f, 1.0f, 1.0f});
        renderizar_circulo(p, e->size * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        renderizar_circulo(p, e->size, (Color){1.0f, 0.35f + e->hitFlash * 0.4f, 0.20f + e->hitFlash * 0.4f, 1.0f}, 20);
        renderizar_estrela(p, e->size * 0.55f, (Color){1.0f, 0.95f, 0.35f, 0.9f});
    }
    renderizar_circulo(e->center, 2.0f, (Color){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void renderizar_projeteis(struct Game *g)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projectile *p = &g->projectiles[i];
        if (!p->active) continue;

        if (p->guidance != GUIDANCE_NONE)
        {
            float angle = atan2f(p->vel.y, p->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Color c;
            if (p->missed)
                c = (Color){1.0f, 0.0f, 0.0f, 1.0f};
            else if (p->guidance == GUIDANCE_APNG)
                c = (Color){1.0f, 0.5f, 0.0f, 1.0f};
            else
                c = (Color){0.8f, 0.0f, 1.0f, 1.0f};

            glPushMatrix();
            glTranslatef(p->pos.x, p->pos.y, 0.0f);
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            renderizar_triangulo(matematica_vetor2d(0, 0), p->radius * 1.4f, c);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(p->pos, p->radius * 2.5f, (Color){c.r, c.g, c.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Color corProjetil = p->fromPlayer ? (Color){0.3f, 0.95f, 1.0f, 0.9f} : (Color){1.0f, 0.8f, 0.0f, 0.9f};
            renderizar_circulo(p->pos, p->radius, corProjetil, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Color corHalo = p->fromPlayer ? (Color){0.3f, 0.85f, 1.0f, 0.28f} : (Color){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(p->pos, p->radius * 2.2f, corHalo, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void renderizar_particulas(struct Game *g)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particle *pt = &g->particles[i];
        if (!pt->active) continue;
        renderizar_circulo(pt->pos, pt->size, (Color){pt->color.r, pt->color.g, pt->color.b, pt->color.a * (pt->life * 2.0f)}, 8);
    }
}
