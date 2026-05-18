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
    int indice_segmento;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (indice_segmento = 0; indice_segmento <= segmentos; ++indice_segmento)
    {
        float angulo_atual = (float)indice_segmento / (float)segmentos * 2.0f * (float)M_PI;
        glVertex2f(vetor.x + cosf(angulo_atual) * raio, vetor.y + sinf(angulo_atual) * raio);
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
    int indice_ponto;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (indice_ponto = 0; indice_ponto <= 10; ++indice_ponto)
    {
        float angulo_atual = (float)indice_ponto / 10.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float raio_atual = (indice_ponto % 2 == 0) ? tamanho : tamanho * 0.45f;
        glVertex2f(vetor.x + cosf(angulo_atual) * raio_atual, vetor.y + sinf(angulo_atual) * raio_atual);
    }
    glEnd();
}

void renderizar_pentagono(Vetor2D vetor, float tamanho, Cor cor)
{
    int indice_vertice;
    glColor4f(cor.r, cor.g, cor.b, cor.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(vetor.x, vetor.y);
    for (indice_vertice = 0; indice_vertice <= 5; ++indice_vertice)
    {
        float angulo_atual = (float)indice_vertice / 5.0f * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        glVertex2f(vetor.x + cosf(angulo_atual) * tamanho, vetor.y + sinf(angulo_atual) * tamanho);
    }
    glEnd();
}

void renderizar_fundo(struct Jogo *jogo)
{
    float deslocamento_fundo = fmodf(jogo->tempo_decorrido * 20.0f, (float)jogo->largura);

    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.04f, 0.06f, 0.11f, 1.0f});

    if (jogo->textura_fundo_carregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, jogo->textura_fundo);
        glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f + deslocamento_fundo / jogo->largura, 0.0f);
        glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f + deslocamento_fundo / jogo->largura, 0.0f);
        glVertex2f((float)jogo->largura, 0.0f);
        glTexCoord2f(1.0f + deslocamento_fundo / jogo->largura, 1.0f);
        glVertex2f((float)jogo->largura, (float)jogo->altura);
        glTexCoord2f(0.0f + deslocamento_fundo / jogo->largura, 1.0f);
        glVertex2f(0.0f, (float)jogo->altura);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    renderizar_circulo(matematica_vetor2d(jogo->largura * 0.85f, jogo->altura * 0.25f), 60.0f, (Cor){0.95f, 0.95f, 0.90f, 0.8f}, 24);
    renderizar_circulo(matematica_vetor2d(jogo->largura * 0.88f, jogo->altura * 0.22f), 58.0f, (Cor){0.04f, 0.06f, 0.11f, 0.9f}, 24);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderizar_retangulo(0.0f, 0.0f, (float)jogo->largura, (float)jogo->altura, (Cor){0.03f, 0.02f, 0.07f, 0.40f});

    {
        int indice_estrela;
        for (indice_estrela = 0; indice_estrela < 18; ++indice_estrela)
        {
            float pos_x_estrela = fmodf((indice_estrela * 97.0f + jogo->tempo_decorrido * (8.0f + indice_estrela)), (float)jogo->largura);
            float pos_y_estrela = 80.0f + fmodf((indice_estrela * 61.0f + jogo->tempo_decorrido * (5.0f + indice_estrela * 0.25f)), (float)jogo->altura - 100.0f);
            renderizar_circulo(matematica_vetor2d(pos_x_estrela, pos_y_estrela), 2.0f + (indice_estrela % 3), (Cor){0.7f, 0.8f, 1.0f, 0.22f}, 10);
        }
    }

    renderizar_retangulo(0.0f, ALTURA_CHAO, (float)jogo->largura, (float)jogo->altura - ALTURA_CHAO, (Cor){0.2f, 0.15f, 0.08f, 1.0f});

    {
        int indice_tabua;
        for (indice_tabua = 0; indice_tabua < 20; ++indice_tabua)
        {
            float pos_x_tabua = indice_tabua * (jogo->largura / 20.0f);
            renderizar_retangulo(pos_x_tabua, ALTURA_CHAO - 4.0f, (jogo->largura / 40.0f), 3.0f, (Cor){0.3f, 0.2f, 0.1f, 0.7f});
        }
    }
}

void renderizar_plataformas(struct Jogo *jogo)
{
    int indice_plataforma;
    for (indice_plataforma = 0; indice_plataforma < MAXIMO_PLATAFORMAS; ++indice_plataforma)
    {
        Obstaculo *plataforma = &jogo->obstaculos[indice_plataforma];
        if (!plataforma->ativo)
            continue;
        renderizar_retangulo(plataforma->x, plataforma->y, plataforma->largura, plataforma->altura, (Cor){0.22f, 0.28f, 0.38f, 0.82f});
        renderizar_retangulo(plataforma->x + 3.0f, plataforma->y + 3.0f, plataforma->largura - 6.0f, plataforma->altura - 6.0f, (Cor){0.35f, 0.46f, 0.62f, 0.35f});
    }
}

void renderizar_jogador(struct Jogo *jogo)
{
    Vetor2D posicao_jogador = jogo->jogador.pos;
    float tamanho_jogador = jogo->jogador.tamanho;

    renderizar_circulo(matematica_vetor2d(posicao_jogador.x, posicao_jogador.y - tamanho_jogador * 0.4f), tamanho_jogador * 0.35f, (Cor){0.95f, 0.85f, 0.70f, 1.0f}, 12);
    renderizar_circulo(matematica_vetor2d(posicao_jogador.x - tamanho_jogador * 0.12f, posicao_jogador.y - tamanho_jogador * 0.5f), tamanho_jogador * 0.08f, (Cor){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_circulo(matematica_vetor2d(posicao_jogador.x + tamanho_jogador * 0.12f, posicao_jogador.y - tamanho_jogador * 0.5f), tamanho_jogador * 0.08f, (Cor){0.2f, 0.2f, 0.2f, 1.0f}, 8);
    renderizar_retangulo(posicao_jogador.x - tamanho_jogador * 0.22f, posicao_jogador.y - tamanho_jogador * 0.08f, tamanho_jogador * 0.44f, tamanho_jogador * 0.35f, (Cor){0.2f, 0.3f, 0.7f, 1.0f});
    renderizar_retangulo(posicao_jogador.x - tamanho_jogador * 0.35f, posicao_jogador.y - tamanho_jogador * 0.05f, tamanho_jogador * 0.25f, tamanho_jogador * 0.12f, (Cor){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(posicao_jogador.x + tamanho_jogador * 0.1f, posicao_jogador.y - tamanho_jogador * 0.05f, tamanho_jogador * 0.25f, tamanho_jogador * 0.12f, (Cor){0.95f, 0.85f, 0.70f, 1.0f});
    renderizar_retangulo(posicao_jogador.x - tamanho_jogador * 0.12f, posicao_jogador.y + tamanho_jogador * 0.28f, tamanho_jogador * 0.15f, tamanho_jogador * 0.25f, (Cor){0.3f, 0.3f, 0.3f, 1.0f});
    renderizar_retangulo(posicao_jogador.x + tamanho_jogador * 0.07f, posicao_jogador.y + tamanho_jogador * 0.28f, tamanho_jogador * 0.15f, tamanho_jogador * 0.25f, (Cor){0.3f, 0.3f, 0.3f, 1.0f});

    if (!jogo->jogador.esta_no_chao)
        renderizar_circulo(posicao_jogador, tamanho_jogador * 0.9f, (Cor){0.3f, 0.9f, 1.0f, 0.2f}, 16);
}

void renderizar_inimigo(struct Inimigo *inimigo)
{
    Vetor2D posicao_inimigo = inimigo_posicao(inimigo);
    if (inimigo->eh_chefao)
    {
        Cor cor_base_chefao = (Cor){0.95f, 0.15f + inimigo->flash_dano * 0.45f, 0.85f, 1.0f};
        renderizar_circulo(posicao_inimigo, inimigo->tamanho, cor_base_chefao, 24);
        renderizar_estrela(posicao_inimigo, inimigo->tamanho * 0.72f, (Cor){1.0f, 0.75f, 1.0f, 0.95f});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        renderizar_circulo(posicao_inimigo, inimigo->tamanho * 1.8f, (Cor){0.95f, 0.2f, 0.95f, 0.20f}, 20);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else if (inimigo->tipo == INIMIGO_TANQUE)
    {
        renderizar_circulo(posicao_inimigo, inimigo->tamanho, (Cor){0.98f, 0.30f + inimigo->flash_dano * 0.25f, 0.18f, 1.0f}, 20);
        renderizar_retangulo(posicao_inimigo.x - inimigo->tamanho * 0.55f, posicao_inimigo.y - inimigo->tamanho * 0.55f, inimigo->tamanho * 1.1f, inimigo->tamanho * 1.1f, (Cor){1.0f, 0.76f, 0.18f, 0.75f});
        renderizar_circulo(posicao_inimigo, inimigo->tamanho * 0.35f, (Cor){0.2f, 0.1f, 0.05f, 0.85f}, 16);
    }
    else if (inimigo->tipo == INIMIGO_ATIRADOR)
    {
        renderizar_losangulo(posicao_inimigo, inimigo->tamanho * 1.05f, (Cor){1.0f, 0.62f + inimigo->flash_dano * 0.25f, 0.24f, 1.0f});
        renderizar_triangulo(matematica_vetor2d(posicao_inimigo.x, posicao_inimigo.y - inimigo->tamanho * 0.1f), inimigo->tamanho * 0.5f, (Cor){1.0f, 0.95f, 0.6f, 0.9f});
    }
    else if (inimigo->tipo == INIMIGO_DIAMANTE)
    {
        float metade_tamanho_diamante = inimigo->tamanho * 0.7f;
        renderizar_circulo(posicao_inimigo, inimigo->tamanho, (Cor){1.0f, 0.45f + inimigo->flash_dano * 0.25f, 0.15f, 1.0f}, 20);
        glPushMatrix();
        glTranslatef(posicao_inimigo.x, posicao_inimigo.y, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        renderizar_retangulo(-metade_tamanho_diamante, -metade_tamanho_diamante, metade_tamanho_diamante * 2.0f, metade_tamanho_diamante * 2.0f, (Cor){1.0f, 0.65f, 0.15f, 0.8f});
        glPopMatrix();
        renderizar_circulo(posicao_inimigo, inimigo->tamanho * 0.45f, (Cor){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else if (inimigo->tipo == INIMIGO_PENTAGONO)
    {
        renderizar_pentagono(posicao_inimigo, inimigo->tamanho, (Cor){0.2f, 0.9f, 1.0f, 1.0f});
        renderizar_circulo(posicao_inimigo, inimigo->tamanho * 0.45f, (Cor){1.0f, 0.8f, 0.2f, 0.85f}, 10);
    }
    else
    {
        renderizar_circulo(posicao_inimigo, inimigo->tamanho, (Cor){1.0f, 0.35f + inimigo->flash_dano * 0.4f, 0.20f + inimigo->flash_dano * 0.4f, 1.0f}, 20);
        renderizar_estrela(posicao_inimigo, inimigo->tamanho * 0.55f, (Cor){1.0f, 0.95f, 0.35f, 0.9f});
    }
    renderizar_circulo(inimigo->centro, 2.0f, (Cor){1.0f, 0.6f, 0.2f, 0.25f}, 8);
}

void renderizar_projeteis(struct Jogo *jogo)
{
    int indice_projetil;
    for (indice_projetil = 0; indice_projetil < MAXIMO_PROJETEIS; ++indice_projetil)
    {
        Projetil *projetil = &jogo->projetis[indice_projetil];
        if (!projetil->ativo)
            continue;

        if (projetil->guianca != GUIANCA_NENHUMA)
        {
            float angulo_graus = atan2f(projetil->vel.y, projetil->vel.x) * 180.0f / (float)M_PI - 90.0f;
            Cor cor_missil;
            if (projetil->errou)
                cor_missil = (Cor){1.0f, 0.0f, 0.0f, 1.0f};
            else if (projetil->guianca == GUIANCA_APN)
                cor_missil = (Cor){1.0f, 0.5f, 0.0f, 1.0f};
            else
                cor_missil = (Cor){0.8f, 0.0f, 1.0f, 1.0f};

            glPushMatrix();
            glTranslatef(projetil->pos.x, projetil->pos.y, 0.0f);
            glRotatef(angulo_graus, 0.0f, 0.0f, 1.0f);
            renderizar_triangulo(matematica_vetor2d(0, 0), projetil->raio * 1.4f, cor_missil);
            glPopMatrix();

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            renderizar_circulo(projetil->pos, projetil->raio * 2.5f, (Cor){cor_missil.r, cor_missil.g, cor_missil.b, 0.25f}, 12);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            Cor cor_bala = projetil->vem_do_jogador ? (Cor){0.3f, 0.95f, 1.0f, 0.9f} : (Cor){1.0f, 0.8f, 0.0f, 0.9f};
            renderizar_circulo(projetil->pos, projetil->raio, cor_bala, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            Cor cor_halo = projetil->vem_do_jogador ? (Cor){0.3f, 0.85f, 1.0f, 0.28f} : (Cor){1.0f, 0.75f, 0.0f, 0.24f};
            renderizar_circulo(projetil->pos, projetil->raio * 2.2f, cor_halo, 14);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
}

void renderizar_particulas(struct Jogo *jogo)
{
    int indice_particula;
    for (indice_particula = 0; indice_particula < MAXIMO_PARTICULAS; ++indice_particula)
    {
        Particula *particula = &jogo->particulas[indice_particula];
        if (!particula->ativo)
            continue;
        renderizar_circulo(particula->pos, particula->tamanho, (Cor){particula->cor.r, particula->cor.g, particula->cor.b, particula->cor.a * (particula->vida * 2.0f)}, 8);
    }
}
