#include "cenario.h"
#include "inimigo.h"
#include "matematica.h"

#include <string.h>

void limpar_entidades(Jogo *g)
{
    memset(g->inimigos, 0, sizeof(g->inimigos));
    memset(g->projetis, 0, sizeof(g->projetis));
    memset(g->particulas, 0, sizeof(g->particulas));
}

void limpar_plataformas(Jogo *g)
{
    memset(g->obstaculos, 0, sizeof(g->obstaculos));
}

void criar_plataformas(Jogo *g)
{
    int count;
    int i, j;
    int validPlacement;
    int attempts;

    limpar_plataformas(g);
    count = 2 + g->onda / 2;
    if (g->dificuldade >= 2)
    {
        count += 1;
    }
    if (count > MAXIMO_PLATAFORMAS)
    {
        count = MAXIMO_PLATAFORMAS;
    }

    for (i = 0; i < count; ++i)
    {
        Obstaculo *o = &g->obstaculos[i];

        attempts = 0;
        do
        {
            validPlacement = 1;
            o->ativo = 1;
            o->largura = matematica_float_aleatorio(100.0f, 220.0f);
            o->altura = matematica_float_aleatorio(22.0f, 44.0f);
            o->x = matematica_float_aleatorio(40.0f, (float)g->largura - o->largura - 40.0f);
            o->y = matematica_float_aleatorio(ALTURA_CHAO - 200.0f, ALTURA_CHAO - 150.0f);

            for (j = 0; j < i; ++j)
            {
                Obstaculo *other = &g->obstaculos[j];
                if (!other->ativo)
                    continue;

                if (!(o->x + o->largura + 20.0f < other->x || o->x > other->x + other->largura + 20.0f ||
                      o->y + o->altura + 20.0f < other->y || o->y > other->y + other->altura + 20.0f))
                {
                    validPlacement = 0;
                    break;
                }
            }

            attempts++;
        } while (!validPlacement && attempts < 10);

        if (!validPlacement)
        {
            o->ativo = 0;
        }
    }
}

void criar_onda(Jogo *g)
{
    int quantidade_inimigos = 3 + g->onda * 1.2;
    int i;
    int finalWave = (g->onda >= g->ondasParaVencer);
    float hpMul = inimigo_multiplicador_vida(g->dificuldade);
    float dmgMul = inimigo_multiplicador_dano(g->dificuldade);
    float fireMul = inimigo_taxa_disparo(g->dificuldade);

    if (finalWave)
    {
        quantidade_inimigos = 1 + g->onda;
    }

    if (quantidade_inimigos > MAXIMO_INIMIGOS)
    {
        quantidade_inimigos = MAXIMO_INIMIGOS;
    }

    limpar_entidades(g);
    criar_plataformas(g);
    g->inimigosRestantes = quantidade_inimigos;

    for (i = 0; i < quantidade_inimigos; ++i)
    {
        Inimigo *e = &g->inimigos[i];
        e->ativo = 1;
        e->ehChefao = (finalWave && i == 0);
        if (e->ehChefao)
        {
            e->tipo = INIMIGO_TANQUE;
        }
        else if (g->onda >= 3 && (i % 6 == 0))
        {
            e->tipo = INIMIGO_PENTAGONO;
        }
        else if (g->onda >= 2 && (i % 5 == 0))
        {
            e->tipo = INIMIGO_DIAMANTE;
        }
        else if (g->onda >= 4 && (i % 7 == 0))
        {
            e->tipo = INIMIGO_TANQUE;
        }
        else if (g->onda >= 1 && (i % 4 == 0))
        {
            e->tipo = INIMIGO_ATIRADOR;
        }
        else
        {
            e->tipo = INIMIGO_PADRAO;
        }

        e->centro = e->ehChefao
                        ? matematica_vetor2d((float)g->largura * 0.5f, (float)g->altura * 0.22f)
                        : matematica_vetor2d(matematica_float_aleatorio(120.0f, (float)g->largura - 120.0f), matematica_float_aleatorio((float)g->altura * 0.14f, (float)g->altura * 0.44f));
        e->raioOrbita = e->ehChefao ? 96.0f : matematica_float_aleatorio(24.0f, 70.0f);
        e->angulo = matematica_float_aleatorio(0.0f, 2.0f * (float)M_PI);
        e->velocidadeAngular = (e->ehChefao ? matematica_float_aleatorio(0.45f, 0.85f) : matematica_float_aleatorio(0.75f, 1.6f)) * ((i % 2 == 0) ? 1.0f : -1.0f) * (1.0f + g->onda * 0.08f);

        if (e->ehChefao)
        {
            e->tamanho = 42.0f;
            e->vida = 420.0f + g->onda * 45.0f;
            e->dano = 24.0f + g->onda * 2.7f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(0.4f, 1.0f);
        }
        else if (e->tipo == INIMIGO_TANQUE)
        {
            e->tamanho = matematica_float_aleatorio(24.0f, 30.0f);
            e->vida = 50.0f + g->onda * 14.0f;
            e->dano = 18.0f + g->onda * 2.6f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(1.3f, 2.8f);
            e->velocidadeAngular *= 0.72f;
        }
        else if (e->tipo == INIMIGO_ATIRADOR)
        {
            e->tamanho = matematica_float_aleatorio(14.0f, 18.0f);
            e->vida = 20.0f + g->onda * 7.5f;
            e->dano = 14.0f + g->onda * 2.0f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(0.6f, 1.4f);
            e->velocidadeAngular *= 1.2f;
            e->raioOrbita += 18.0f;
        }
        else if (e->tipo == INIMIGO_DIAMANTE)
        {
            e->tamanho = matematica_float_aleatorio(18.0f, 22.0f);
            e->vida = 35.0f + g->onda * 10.0f;
            e->dano = 15.0f + g->onda * 2.0f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(2.0f, 4.0f);
            e->contadorRajada = 2;
        }
        else if (e->tipo == INIMIGO_PENTAGONO)
        {
            e->tamanho = matematica_float_aleatorio(20.0f, 25.0f);
            e->vida = 45.0f + g->onda * 12.0f;
            e->dano = 20.0f + g->onda * 2.5f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(3.0f, 5.0f);
        }
        else
        {
            e->tamanho = matematica_float_aleatorio(16.0f, 24.0f);
            e->vida = 26.0f + g->onda * 9.0f;
            e->dano = 16.0f + g->onda * 2.2f;
            e->tempoRecargaDisparo = matematica_float_aleatorio(0.8f, 2.2f);
        }

        e->vidaMaxima = e->vida;
        e->vida *= hpMul;
        e->vidaMaxima = e->vida;
        e->dano *= dmgMul;
        e->tempoRecargaDisparo *= fireMul;
        e->flashDano = 0.0f;

        e->vel = matematica_vetor2d(0.0f, 0.0f);
        e->aceleracao = matematica_vetor2d(0.0f, 0.0f);
    }
}