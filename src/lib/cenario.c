#include "cenario.h"
#include "inimigo.h"
#include "matematica.h"

#include <string.h>

void cenario_limpar_entidades(Jogo *jogo)
{
    memset(jogo->inimigos, 0, sizeof(jogo->inimigos));
    memset(jogo->projetis, 0, sizeof(jogo->projetis));
    memset(jogo->particulas, 0, sizeof(jogo->particulas));
}

void cenario_limpar_plataformas(Jogo *jogo)
{
    memset(jogo->obstaculos, 0, sizeof(jogo->obstaculos));
}

void cenario_criar_plataformas(Jogo *jogo)
{
    int count;
    int i, j;
    int validPlacement;
    int attempts;

    cenario_limpar_plataformas(jogo);
    count = 2 + jogo->onda / 2;
    if (jogo->dificuldade >= 2)
    {
        count += 1;
    }
    if (count > MAXIMO_PLATAFORMAS)
    {
        count = MAXIMO_PLATAFORMAS;
    }

    for (i = 0; i < count; ++i)
    {
        Obstaculo *o = &jogo->obstaculos[i];

        attempts = 0;
        do
        {
            validPlacement = 1;
            o->ativo = 1;
            o->largura = matematica_float_aleatorio_alcance(100.0f, 220.0f);
            o->altura = matematica_float_aleatorio_alcance(22.0f, 44.0f);
            o->x = matematica_float_aleatorio_alcance(40.0f, (float)jogo->largura - o->largura - 40.0f);
            o->y = matematica_float_aleatorio_alcance(ALTURA_CHAO - 200.0f, ALTURA_CHAO - 150.0f);

            for (j = 0; j < i; ++j)
            {
                Obstaculo *other = &jogo->obstaculos[j];
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

void cenario_criar_onda(Jogo *jogo)
{
    int quantidade_inimigos = 3 + jogo->onda * 1.2;
    int i;
    int finalWave = (jogo->onda >= jogo->ondas_para_vencer);
    float hpMul = inimigo_multiplicador_vida(jogo->dificuldade);
    float dmgMul = inimigo_multiplicador_dano(jogo->dificuldade);
    float fireMul = inimigo_taxa_disparo(jogo->dificuldade);

    if (finalWave)
    {
        quantidade_inimigos = 1 + jogo->onda;
    }

    if (quantidade_inimigos > MAXIMO_INIMIGOS)
    {
        quantidade_inimigos = MAXIMO_INIMIGOS;
    }

    cenario_limpar_entidades(jogo);
    cenario_criar_plataformas(jogo);
    jogo->inimigos_restantes = quantidade_inimigos;

    for (i = 0; i < quantidade_inimigos; ++i)
    {
        Inimigo *e = &jogo->inimigos[i];
        e->ativo = 1;
        e->eh_chefao = (finalWave && i == 0);
        if (e->eh_chefao)
        {
            e->tipo = INIMIGO_TANQUE;
        }
        else if (jogo->onda >= 3 && (i % 6 == 0))
        {
            e->tipo = INIMIGO_PENTAGONO;
        }
        else if (jogo->onda >= 2 && (i % 5 == 0))
        {
            e->tipo = INIMIGO_DIAMANTE;
        }
        else if (jogo->onda >= 4 && (i % 7 == 0))
        {
            e->tipo = INIMIGO_TANQUE;
        }
        else if (jogo->onda >= 1 && (i % 4 == 0))
        {
            e->tipo = INIMIGO_ATIRADOR;
        }
        else
        {
            e->tipo = INIMIGO_PADRAO;
        }

        e->centro = e->eh_chefao
                        ? matematica_vetor2d((float)jogo->largura * 0.5f, (float)jogo->altura * 0.22f)
                        : matematica_vetor2d(matematica_float_aleatorio_alcance(120.0f, (float)jogo->largura - 120.0f), matematica_float_aleatorio_alcance((float)jogo->altura * 0.14f, (float)jogo->altura * 0.44f));
        e->raio_orbita = e->eh_chefao ? 96.0f : matematica_float_aleatorio_alcance(24.0f, 70.0f);
        e->angulo = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
        e->velocidade_angular = (e->eh_chefao ? matematica_float_aleatorio_alcance(0.45f, 0.85f) : matematica_float_aleatorio_alcance(0.75f, 1.6f)) * ((i % 2 == 0) ? 1.0f : -1.0f) * (1.0f + jogo->onda * 0.08f);

        if (e->eh_chefao)
        {
            e->tamanho = 42.0f;
            e->vida = 420.0f + jogo->onda * 45.0f;
            e->dano = 24.0f + jogo->onda * 2.7f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.4f, 1.0f);
        }
        else if (e->tipo == INIMIGO_TANQUE)
        {
            e->tamanho = matematica_float_aleatorio_alcance(24.0f, 30.0f);
            e->vida = 50.0f + jogo->onda * 14.0f;
            e->dano = 18.0f + jogo->onda * 2.6f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.3f, 2.8f);
            e->velocidade_angular *= 0.72f;
        }
        else if (e->tipo == INIMIGO_ATIRADOR)
        {
            e->tamanho = matematica_float_aleatorio_alcance(14.0f, 18.0f);
            e->vida = 20.0f + jogo->onda * 7.5f;
            e->dano = 14.0f + jogo->onda * 2.0f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.6f, 1.4f);
            e->velocidade_angular *= 1.2f;
            e->raio_orbita += 18.0f;
        }
        else if (e->tipo == INIMIGO_DIAMANTE)
        {
            e->tamanho = matematica_float_aleatorio_alcance(18.0f, 22.0f);
            e->vida = 35.0f + jogo->onda * 10.0f;
            e->dano = 15.0f + jogo->onda * 2.0f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(2.0f, 4.0f);
            e->contador_rajada = 2;
        }
        else if (e->tipo == INIMIGO_PENTAGONO)
        {
            e->tamanho = matematica_float_aleatorio_alcance(20.0f, 25.0f);
            e->vida = 45.0f + jogo->onda * 12.0f;
            e->dano = 20.0f + jogo->onda * 2.5f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 5.0f);
        }
        else
        {
            e->tamanho = matematica_float_aleatorio_alcance(16.0f, 24.0f);
            e->vida = 26.0f + jogo->onda * 9.0f;
            e->dano = 16.0f + jogo->onda * 2.2f;
            e->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.8f, 2.2f);
        }

        e->vida_maxima = e->vida;
        e->vida *= hpMul;
        e->vida_maxima = e->vida;
        e->dano *= dmgMul;
        e->tempo_recarga_disparo *= fireMul;
        e->flash_dano = 0.0f;

        e->vel = matematica_vetor2d(0.0f, 0.0f);
        e->aceleracao = matematica_vetor2d(0.0f, 0.0f);
    }
}
