#include "particulas.h"
#include "matematica.h"

#include <math.h>

void particulas_criar(Jogo *jogo, Vetor2D vetor, int quantidade, Cor cor)
{
    int indice_criacao;
    for (indice_criacao = 0; indice_criacao < quantidade; ++indice_criacao)
    {
        int indice_particula;
        for (indice_particula = 0; indice_particula < MAXIMO_PARTICULAS; ++indice_particula)
        {
            Particula *particula = &jogo->particulas[indice_particula];
            if (!particula->ativo)
            {
                float angulo_aleatorio = matematica_float_aleatorio_alcance(0.0f, 2.0f * (float)M_PI);
                float velocidade_aleatoria = matematica_float_aleatorio_alcance(30.0f, 180.0f);
                particula->ativo = 1;
                particula->pos = vetor;
                particula->vel = matematica_vetor2d(cosf(angulo_aleatorio) * velocidade_aleatoria, sinf(angulo_aleatorio) * velocidade_aleatoria);
                particula->tamanho = matematica_float_aleatorio_alcance(2.0f, 5.0f);
                particula->vida = matematica_float_aleatorio_alcance(0.2f, 0.6f);
                particula->cor = cor;
                break;
            }
        }
    }
}
