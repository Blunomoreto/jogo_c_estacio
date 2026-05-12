#include "renderizar.h"
#include "jogo.h"
#include "inimigo.h"
#include "matematica.h"

#include <GL/glut.h>
#include <math.h>

void renderizar_perspectiva_ortografica(struct Jogo *g)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)g->largura, (GLdouble)g->altura, 0.0);
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

void renderizar_fundo(struct Jogo *g)
{
    float offset = fmodf(g->tempoDecorrido * 20.0f, (float)g->largura);

    if (g->texturaFundoCarregada)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g->texturaFundo);
        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + offset / g->largura, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + offset / g->largura, 0.0f); glVertex2f((float)g->largura, 0.0f);
        glTexCoord2f(1.0f + offset / g->largura, 1.0f); glVertex2f((float)g->largura, (float)g->altura);
        glTexCoord2f(0.0f + offset / g->largura, 1.0f); glVertex2f(0.0f, (float)g->altura);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    renderizar_retangulo(0.0f, 0.0f, (float)g->largura, (float)g->altura, (Color){0.04f, 0.06f, 0.11f, 1.0f});

    renderizar_circulo(matematica_vetor2d(g->largura * 0.85f, g->altura * 0.25f), 60.0f, (Color){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    renderizar_circulo(matematica_vetor2d(g->largura * 0.88f, g->altura * 0.22f), 58.0f, (Color){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderizar_retangulo(0.0f, 0.0f, (float)g->largura, (float)g->altura, (Color){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int i;
        for (i = 0; i < 18; ++i)
        {
            float x = fmodf((i * 97.0f + g->tempoDecorrido * (8.0f + i)), (float)g->largura);
            float y = 80.0f + fmodf((i * 61.0f + g->tempoDecorrido * (5.0f + i * 0.25f)), (float)g->altura - 100.0f);
            renderizar_circulo(matematica_vetor2d(x, y), 2.0f + (i % 3), (Color){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    renderizar_retangulo(0.0f, ALTURA_CHAO, (float)g->largura, (float)g->altura - ALTURA_CHAO, (Color){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int i;
        for (i = 0; i < 20; ++i)
        {
            float x1 = i * (g->largura / 20.0f);
            renderizar_retangulo(x1, ALTURA_CHAO - 4.0f, (g->largura / 40.0f), 3.0f, (Color){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}

void renderizar_plataformas(struct Jogo *g)
{
    int i;
    for (i = 0; i < MAXIMO_PLATAFORMAS; ++i)
    {
        Obstaculo *o = &g->obstaculos[i];
        if (!o->ativo) continue;
        renderizar_retangulo(o->x, o->y, o->largura, o->altura, (Color){0.22f, 0.28f, 0.38f, 0.82f});
        renderizar_retangulo(o->x + 3.0f, o->y + 3.0f, o->largura - 6.0f, o->altura - 6.0f, (Color){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void renderizar_jogador(struct Jogo *g)
{
    Vetor2D p = g->jogador.pos;
    float size = g->jogador.tamanho;

    renderizar_circulo(matematica_vetor2d(p.x, p.y - size * 0.4f), size * 0.35f, (Color){0.95f, 0.85f, 0.70f, 1.0f}, 12);
    renderizar_circulo(matematica_vetor2d(p.x - size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_circulo(matematica_vetor2d(p.x + size * 0.12f, p.y - size * 0.5f), size * 0.08f, (Color){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_retangulo(p.x - size * 0.22f, p.y - size * 0.08f, size * 0.44f, size * 0.35f, (Color){0.2f, 0.3f, 0.7f, 1.0f});
    renderizar_retangulo(p.x - size * 0.35f, p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(p.x + size * 0.1f,  p.y - size * 0.05f, size * 0.25f, size * 0.12f, (Color){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(p.x - size * 0.12f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});
    renderizar_retangulo(p.x + size * 0.07f, p.y + size * 0.28f, size * 0.15f, size * 0.25f, (Color){0.3f, 0.3f, 0.3f, 1.0f});

    if (!g->jogador.estaNoChao)
        renderizar_circulo(p, size * 0.9f, (Color){0.3f, 0.9f, 1.0f, 0.2f}, 16);
}

void renderizar_inimigo(struct Inimigo *e)
{
    Vetor2D p = inimigo_posicao(e);
    if (e->ehChefao)
    {
        Color base = (Color){0.95f, 0.15f + e->flashDano * 0.45f, 0.85f, 1.0f};
        renderizar_circulo(p, e->tamanho, base, 24);
        renderizar_estrela(p, e->tamanho * 0.72f, (Color){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderizar_circulo(p, e->tamanho * 1.8f, (Color){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (e->tipo == INIMIGO_TANQUE)
    {
        renderizar_circulo(p, e->tamanho, (Color){0.98f, 0.30f + e->flashDano * 0.25f, 0.18f, 1.0f}, 20);
        renderizar_retangulo(p.x - e->tamanho * 0.55f, p.y - e->tamanho * 0.55f, e->tamanho * 1.1f, e->tamanho * 1.1f, (Color){1.0f, 0.76f, 0.18f, 0.75f});
        renderizar_circulo(p, e->tamanho * 0.35f, (Color){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (e->tipo == INIMIGO_ATIRADOR)
    {
        renderizar_losangulo(p, e->tamanho * 1.05f, (Color){1.0f, 0.62f + e->flashDano * 0.25f, 0.24f, 1.0f});
        renderizar_triangulo(matematica_vetor2d(p.x, p.y - e->tamanho * 0.1f), e->tamanho * 0.5f, (Color){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (e->tipo == INIMIGO_DIAMANTE)
    {
        float s = e->tamanho * 0.7f;
        renderizar_circulo(p, e->tamanho, (Color){1.0f, 0.45f + e->flashDano * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(p.x, p.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        renderizar_retangulo(-s, -s, s * 2.0f, s * 2.0f, (Color){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        renderizar_circulo(p, e->tamanho * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (e->tipo == INIMIGO_PENTAGONO)
    {
        renderizar_pentagono(p, e->tamanho, (Color){0.2f, 0.9f, 1.0f, 1.0f});
        renderizar_circulo(p, e->tamanho * 0.45f, (Color){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        renderizar_circulo(p, e->tamanho, (Color){1.0f, 0.35f + e->flashDano * 0.4f, 0.20f + e->flashDano * 0.4f, 1.0f}, 20);
        renderizar_estrela(p, e->tamanho * 0.55f, (Color){1.0f, 0.95f, 0.35f, 0.9f});
    }
    renderizar_circulo(e->centro, 2.0f, (Color){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void renderizar_projeteis(struct Jogo *g)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &g->projetis[i];
        if (!p->ativo) continue;

        if (p->orientacao != ORIENTACAO_NENHUMA)
        {
            float angle = atan2f(p->vel.y, p->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Color c;
            if (p->errou)
                c = (Color){1.0f, 0.0f, 0.0f, 1.0f};
            else if (p->orientacao == ORIENTACAO_ANGULO_PONTO)
                c = (Color){1.0f, 0.5f, 0.0f, 1.0f};
            else
                c = (Color){0.8f, 0.0f, 1.0f, 1.0f};

            glPushMatrix();
            glTranslatef(p->pos.x, p->pos.y, 0.0f);
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            renderizar_triangulo(matematica_vetor2d(0, 0), p->raio * 1.4f, c);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(p->pos, p->raio * 2.5f, (Color){c.r, c.g, c.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Color corProjetil = p->vemDoJogador ? (Color){0.3f, 0.95f, 1.0f, 0.9f} : (Color){1.0f, 0.8f, 0.0f, 0.9f};
            renderizar_circulo(p->pos, p->raio, corProjetil, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Color corHalo = p->vemDoJogador ? (Color){0.3f, 0.85f, 1.0f, 0.28f} : (Color){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(p->pos, p->raio * 2.2f, corHalo, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void renderizar_particulas(struct Jogo *g)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particula *pt = &g->particulas[i];
        if (!pt->ativo) continue;
        renderizar_circulo(pt->pos, pt->tamanho, (Color){pt->cor.r, pt->cor.g, pt->cor.b, pt->cor.a * (pt->vida * 2.0f)}, 8);
    }
}