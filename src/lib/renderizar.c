#include "renderizar.h"
#include "jogo.h"
#include "inimigo.h"
#include "matematica.h"

#include <GL/glut.h>
#include <math.h>

void renderizar_perspectiva_ortografica(struct Jogo *jogo)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)jogo->largura, (GLdouble)jogo->altura, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderizar_retangulo(float x, float y, float largura, float altura, Cor cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + largura, y);
    glVertex2f(x + largura, y + altura);
    glVertex2f(x, y + altura);
    glEnd();
}

void renderizar_circulo(Vetor2D vetor, float raio, Cor cor, int segmentos)
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

void renderizar_triangulo(Vetor2D vetor, float tamanho, Cor cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLES);
    glVertex2f(vetor.x, vetor.y + tamanho);
    glVertex2f(vetor.x - tamanho * 0.8f, vetor.y - tamanho * 0.7f);
    glVertex2f(vetor.x + tamanho * 0.8f, vetor.y - tamanho * 0.7f);
    glEnd();
}

void renderizar_losangulo(Vetor2D vetor, float tamanho, Cor cor)
{
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_QUADS);
    glVertex2f(vetor.x, vetor.y + tamanho);
    glVertex2f(vetor.x + tamanho, vetor.y);
    glVertex2f(vetor.x, vetor.y - tamanho);
    glVertex2f(vetor.x - tamanho, vetor.y);
    glEnd();
}

void renderizar_estrela(Vetor2D vetor, float tamanho, Cor cor)
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

void renderizar_pentagono(Vetor2D vetor, float tamanho, Cor cor)
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

void renderizar_fundo(struct Jogo *jogo)
{
    float offset = fmodf(jogo->tempo_decorrido * 20.0f, (float)jogo->largura);

    if (jogo->textura_fundo_carregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, jogo->textura_fundo);
        glColor4f(1.0f, 1.0f, 1.0f, 0.28f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + offset / jogo->largura, 0.0f);
        glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + offset / jogo->largura, 0.0f);
        glVertex2f((float)jogo->largura, 0.0f);
        glTexCoord2f(1.0f + offset / jogo->largura, 1.0f);
        glVertex2f((float)jogo->largura, (float)jogo->altura);
        glTexCoord2f(0.0f + offset / jogo->largura, 1.0f);
        glVertex2f(0.0f, (float)jogo->altura);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.04f, 0.06f, 0.11f, 1.0f});

    renderizar_circulo(matematica_vetor2d(jogo->largura * 0.85f, jogo->altura * 0.25f), 60.0f, (Cor){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    renderizar_circulo(matematica_vetor2d(jogo->largura * 0.88f, jogo->altura * 0.22f), 58.0f, (Cor){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int i;
        for (i = 0; i < 18; ++i)
        {
            float x = fmodf((i * 97.0f + jogo->tempo_decorrido * (8.0f + i)), (float)jogo->largura);
            float y = 80.0f + fmodf((i * 61.0f + jogo->tempo_decorrido * (5.0f + i * 0.25f)), (float)jogo->altura - 100.0f);
            renderizar_circulo(matematica_vetor2d(x, y), 2.0f + (i % 3), (Cor){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    renderizar_retangulo(0.0f, ALTURA_CHAO, (float)jogo->largura, (float)jogo->altura - ALTURA_CHAO, (Cor){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int i;
        for (i = 0; i < 20; ++i)
        {
            float x1 = i * (jogo->largura / 20.0f);
            renderizar_retangulo(x1, ALTURA_CHAO - 4.0f, (jogo->largura / 40.0f), 3.0f, (Cor){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}

void renderizar_plataformas(struct Jogo *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PLATAFORMAS; ++i)
    {
        Obstaculo *o = &jogo->obstaculos[i];
        if (!o->ativo)
            continue;
        renderizar_retangulo(o->x, o->y, o->largura, o->altura, (Cor){0.22f, 0.28f, 0.38f, 0.82f});
        renderizar_retangulo(o->x + 3.0f, o->y + 3.0f, o->largura - 6.0f, o->altura - 6.0f, (Cor){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void renderizar_jogador(struct Jogo *jogo)
{
    Vetor2D vetor = jogo->jogador.pos;
    float tamanho = jogo->jogador.tamanho;

    renderizar_circulo(matematica_vetor2d(vetor.x, vetor.y - tamanho * 0.4f), tamanho * 0.35f, (Cor){0.95f, 0.85f, 0.70f, 1.0f}, 12);
    renderizar_circulo(matematica_vetor2d(vetor.x - tamanho * 0.12f, vetor.y - tamanho * 0.5f), tamanho * 0.08f, (Cor){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_circulo(matematica_vetor2d(vetor.x + tamanho * 0.12f, vetor.y - tamanho * 0.5f), tamanho * 0.08f, (Cor){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_retangulo(vetor.x - tamanho * 0.22f, vetor.y - tamanho * 0.08f, tamanho * 0.44f, tamanho * 0.35f, (Cor){0.2f, 0.3f, 0.7f, 1.0f});
    renderizar_retangulo(vetor.x - tamanho * 0.35f, vetor.y - tamanho * 0.05f, tamanho * 0.25f, tamanho * 0.12f, (Cor){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(vetor.x + tamanho * 0.1f, vetor.y - tamanho * 0.05f, tamanho * 0.25f, tamanho * 0.12f, (Cor){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(vetor.x - tamanho * 0.12f, vetor.y + tamanho * 0.28f, tamanho * 0.15f, tamanho * 0.25f, (Cor){0.3f, 0.3f, 0.3f, 1.0f});
    renderizar_retangulo(vetor.x + tamanho * 0.07f, vetor.y + tamanho * 0.28f, tamanho * 0.15f, tamanho * 0.25f, (Cor){0.3f, 0.3f, 0.3f, 1.0f});

    if (!jogo->jogador.esta_no_chao)
        renderizar_circulo(vetor, tamanho * 0.9f, (Cor){0.3f, 0.9f, 1.0f, 0.2f}, 16);
}

void renderizar_inimigo(struct Inimigo *inimigo)
{
    Vetor2D vetor = inimigo_posicao(inimigo);
    if (inimigo->eh_chefao)
    {
        Cor base = (Cor){0.95f, 0.15f + inimigo->flash_dano * 0.45f, 0.85f, 1.0f};
        renderizar_circulo(vetor, inimigo->tamanho, base, 24);
        renderizar_estrela(vetor, inimigo->tamanho * 0.72f, (Cor){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderizar_circulo(vetor, inimigo->tamanho * 1.8f, (Cor){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (inimigo->tipo == INIMIGO_TANQUE)
    {
        renderizar_circulo(vetor, inimigo->tamanho, (Cor){0.98f, 0.30f + inimigo->flash_dano * 0.25f, 0.18f, 1.0f}, 20);
        renderizar_retangulo(vetor.x - inimigo->tamanho * 0.55f, vetor.y - inimigo->tamanho * 0.55f, inimigo->tamanho * 1.1f, inimigo->tamanho * 1.1f, (Cor){1.0f, 0.76f, 0.18f, 0.75f});
        renderizar_circulo(vetor, inimigo->tamanho * 0.35f, (Cor){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (inimigo->tipo == INIMIGO_ATIRADOR)
    {
        renderizar_losangulo(vetor, inimigo->tamanho * 1.05f, (Cor){1.0f, 0.62f + inimigo->flash_dano * 0.25f, 0.24f, 1.0f});
        renderizar_triangulo(matematica_vetor2d(vetor.x, vetor.y - inimigo->tamanho * 0.1f), inimigo->tamanho * 0.5f, (Cor){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (inimigo->tipo == INIMIGO_DIAMANTE)
    {
        float s = inimigo->tamanho * 0.7f;
        renderizar_circulo(vetor, inimigo->tamanho, (Cor){1.0f, 0.45f + inimigo->flash_dano * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(vetor.x, vetor.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        renderizar_retangulo(-s, -s, s * 2.0f, s * 2.0f, (Cor){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        renderizar_circulo(vetor, inimigo->tamanho * 0.45f, (Cor){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (inimigo->tipo == INIMIGO_PENTAGONO)
    {
        renderizar_pentagono(vetor, inimigo->tamanho, (Cor){0.2f, 0.9f, 1.0f, 1.0f});
        renderizar_circulo(vetor, inimigo->tamanho * 0.45f, (Cor){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        renderizar_circulo(vetor, inimigo->tamanho, (Cor){1.0f, 0.35f + inimigo->flash_dano * 0.4f, 0.20f + inimigo->flash_dano * 0.4f, 1.0f}, 20);
        renderizar_estrela(vetor, inimigo->tamanho * 0.55f, (Cor){1.0f, 0.95f, 0.35f, 0.9f});
    }
    renderizar_circulo(inimigo->centro, 2.0f, (Cor){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void renderizar_projeteis(struct Jogo *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PROJETEIS; ++i)
    {
        Projetil *p = &jogo->projetis[i];
        if (!p->ativo)
            continue;

        if (p->guianca != GUIANCA_NENHUMA)
        {
            float angle = atan2f(p->vel.y, p->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Cor cor;
            if (p->errou)
                cor = (Cor){1.0f, 0.0f, 0.0f, 1.0f};
            else if (p->guianca == GUIANCA_APN)
                cor = (Cor){1.0f, 0.5f, 0.0f, 1.0f};
            else
                cor = (Cor){0.8f, 0.0f, 1.0f, 1.0f};

            glPushMatrix();
            glTranslatef(p->pos.x, p->pos.y, 0.0f);
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            renderizar_triangulo(matematica_vetor2d(0, 0), p->raio * 1.4f, cor);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(p->pos, p->raio * 2.5f, (Cor){cor.r, cor.g, cor.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Cor corProjetil = p->vem_do_jogador ? (Cor){0.3f, 0.95f, 1.0f, 0.9f} : (Cor){1.0f, 0.8f, 0.0f, 0.9f};
            renderizar_circulo(p->pos, p->raio, corProjetil, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Cor corHalo = p->vem_do_jogador ? (Cor){0.3f, 0.85f, 1.0f, 0.28f} : (Cor){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(p->pos, p->raio * 2.2f, corHalo, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void renderizar_particulas(struct Jogo *jogo)
{
    int i;
    for (i = 0; i < MAXIMO_PARTICULAS; ++i)
    {
        Particula *pt = &jogo->particulas[i];
        if (!pt->ativo)
            continue;
        renderizar_circulo(pt->pos, pt->tamanho, (Cor){pt->cor.r, pt->cor.g, pt->cor.b, pt->cor.a * (pt->vida * 2.0f)}, 8);
    }
}
