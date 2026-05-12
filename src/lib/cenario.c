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
    int quantidade_plataformas;
    int indice_plataforma;
    int indice_verificacao;
    int posicionamento_valido;
    int tentativas;

    cenario_limpar_plataformas(jogo);
    quantidade_plataformas = 2 + jogo->onda / 2;
    if (jogo->dificuldade >= 2)
    {
        quantidade_plataformas += 1;
    }
    if (quantidade_plataformas > MAXIMO_PLATAFORMAS)
    {
        quantidade_plataformas = MAXIMO_PLATAFORMAS;
    }

    for (indice_plataforma = 0; indice_plataforma < quantidade_plataformas; ++indice_plataforma)
    {
        Obstaculo *plataforma = &jogo->obstaculos[indice_plataforma];

        tentativas = 0;
        do
        {
            posicionamento_valido = 1;
            plataforma->ativo = 1;
            plataforma->largura = matematica_float_aleatorio_alcance(100.0f, 220.0f);
            plataforma->altura = matematica_float_aleatorio_alcance(22.0f, 44.0f);
            plataforma->x = matematica_float_aleatorio_alcance(40.0f, (float)jogo->largura - plataforma->largura - 40.0f);
            plataforma->y = matematica_float_aleatorio_alcance(ALTURA_CHAO - 200.0f, ALTURA_CHAO - 150.0f);

            for (indice_verificacao = 0; indice_verificacao < indice_plataforma; ++indice_verificacao)
            {
                Obstaculo *plataforma_verificada = &jogo->obstaculos[indice_verificacao];
                if (!plataforma_verificada->ativo)
                    continue;

                if (!(plataforma->x + plataforma->largura + 20.0f < plataforma_verificada->x ||
                      plataforma->x > plataforma_verificada->x + plataforma_verificada->largura + 20.0f ||
                      plataforma->y + plataforma->altura + 20.0f < plataforma_verificada->y ||
                      plataforma->y > plataforma_verificada->y + plataforma_verificada->altura + 20.0f))
                {
                    posicionamento_valido = 0;
                    break;
                }
            }

            tentativas++;
        } while (!posicionamento_valido && tentativas < 10);

        if (!posicionamento_valido)
        {
            plataforma->ativo = 0;
        }
    }
}

void cenario_criar_onda(Jogo *jogo)
{
    int quantidade_inimigos = 3 + jogo->onda * 1.2;
    int indice_inimigo;
    int onda_final = (jogo->onda >= jogo->ondas_para_vencer);
    float multiplicador_vida = inimigo_multiplicador_vida(jogo->dificuldade);
    float multiplicador_dano = inimigo_multiplicador_dano(jogo->dificuldade);
    float multiplicador_taxa_disparo = inimigo_taxa_disparo(jogo->dificuldade);

    if (onda_final)
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

    for (indice_inimigo = 0; indice_inimigo < quantidade_inimigos; ++indice_inimigo)
    {
        Inimigo *inimigo = &jogo->inimigos[indice_inimigo];
        inimigo->ativo = 1;
        inimigo->eh_chefao = (onda_final && indice_inimigo == 0);
        if (inimigo->eh_chefao)
        {
            inimigo->tipo = INIMIGO_TANQUE;
        }
        else if (jogo->onda >= 3 && (indice_inimigo % 6 == 0))
        {
            inimigo->tipo = INIMIGO_PENTAGONO;
        }
        else if (jogo->onda >= 2 && (indice_inimigo % 5 == 0))
        {
            inimigo->tipo = INIMIGO_DIAMANTE;
        }
        else if (jogo->onda >= 4 && (indice_inimigo % 7 == 0))
        {
            inimigo->tipo = INIMIGO_TANQUE;
        }
        else if (jogo->onda >= 1 && (indice_inimigo % 4 == 0))
        {
            inimigo->tipo = INIMIGO_ATIRADOR;
        }
        else
        {
            inimigo->tipo = INIMIGO_PADRAO;
        }

        inimigo->centro = inimigo->eh_chefao
                        ? matematica_vetor2d((float)jogo->largura * 0.5f, (float)jogo->altura * 0.22f)
                        : matematica_vetor2d(matematica_float_aleatorio_alcance(120.0f, (float)jogo->largura - 120.0f), matematica_float_aleatorio_alcance((float)jogo->altura * 0.14f, (float)jogo->altura * 0.44f));
        inimigo->raio_orbita = inimigo->eh_chefao ? 96.0f : matematica_float_aleatorio_alcance(24.0f, 70.0f);
        inimigo->angulo = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
        inimigo->velocidade_angular = (inimigo->eh_chefao ? matematica_float_aleatorio_alcance(0.45f, 0.85f) : matematica_float_aleatorio_alcance(0.75f, 1.6f)) * ((indice_inimigo % 2 == 0) ? 1.0f : -1.0f) * (1.0f + jogo->onda * 0.08f);

        if (inimigo->eh_chefao)
        {
            inimigo->tamanho = 42.0f;
            inimigo->vida = 420.0f + jogo->onda * 45.0f;
            inimigo->dano = 24.0f + jogo->onda * 2.7f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.4f, 1.0f);
        }
        else if (inimigo->tipo == INIMIGO_TANQUE)
        {
            inimigo->tamanho = matematica_float_aleatorio_alcance(24.0f, 30.0f);
            inimigo->vida = 50.0f + jogo->onda * 14.0f;
            inimigo->dano = 18.0f + jogo->onda * 2.6f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(1.3f, 2.8f);
            inimigo->velocidade_angular *= 0.72f;
        }
        else if (inimigo->tipo == INIMIGO_ATIRADOR)
        {
            inimigo->tamanho = matematica_float_aleatorio_alcance(14.0f, 18.0f);
            inimigo->vida = 20.0f + jogo->onda * 7.5f;
            inimigo->dano = 14.0f + jogo->onda * 2.0f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.6f, 1.4f);
            inimigo->velocidade_angular *= 1.2f;
            inimigo->raio_orbita += 18.0f;
        }
        else if (inimigo->tipo == INIMIGO_DIAMANTE)
        {
            inimigo->tamanho = matematica_float_aleatorio_alcance(18.0f, 22.0f);
            inimigo->vida = 35.0f + jogo->onda * 10.0f;
            inimigo->dano = 15.0f + jogo->onda * 2.0f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(2.0f, 4.0f);
            inimigo->contador_rajada = 2;
        }
        else if (inimigo->tipo == INIMIGO_PENTAGONO)
        {
            inimigo->tamanho = matematica_float_aleatorio_alcance(20.0f, 25.0f);
            inimigo->vida = 45.0f + jogo->onda * 12.0f;
            inimigo->dano = 20.0f + jogo->onda * 2.5f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(3.0f, 5.0f);
        }
        else
        {
            inimigo->tamanho = matematica_float_aleatorio_alcance(16.0f, 24.0f);
            inimigo->vida = 26.0f + jogo->onda * 9.0f;
            inimigo->dano = 16.0f + jogo->onda * 2.2f;
            inimigo->tempo_recarga_disparo = matematica_float_aleatorio_alcance(0.8f, 2.2f);
        }

        inimigo->vida_maxima = inimigo->vida;
        inimigo->vida *= multiplicador_vida;
        inimigo->vida_maxima = inimigo->vida;
        inimigo->dano *= multiplicador_dano;
        inimigo->tempo_recarga_disparo *= multiplicador_taxa_disparo;
        inimigo->flash_dano = 0.0f;

        inimigo->vel = matematica_vetor2d(0.0f, 0.0f);
        inimigo->aceleracao = matematica_vetor2d(0.0f, 0.0f);
    }
}
