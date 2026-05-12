#include "inimigo.h"
#include "matematica.h"

#include <math.h>

Vetor2D inimigo_posicao(const Inimigo *inimigo)
{
    return matematica_vetor2d(inimigo->centro.x + cosf(inimigo->angulo) * inimigo->raio_orbita,
                              inimigo->centro.y + sinf(inimigo->angulo) * inimigo->raio_orbita);
}

const char *inimigo_nome_dificuldade(int dificuldade)
{
    if (dificuldade <= 0)
        return "EASY";
    if (dificuldade >= 2)
        return "HARD";
    return "NORMAL";
}

float inimigo_multiplicador_vida(int dificuldade)
{
    if (dificuldade <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (dificuldade >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_multiplicador_dano(int dificuldade)
{
    if (dificuldade <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (dificuldade >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_multiplicador_pontos(int dificuldade)
{
    if (dificuldade <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (dificuldade >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_taxa_disparo(int dificuldade)
{
    if (dificuldade <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (dificuldade >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_tempo_inicio(int dificuldade)
{
    if (dificuldade <= 0)
        return TEMPO_LIMITE + DIFICULDADE_FACIL_AJUSTE_TEMPO_LIMITE;
    if (dificuldade >= 2)
        return TEMPO_LIMITE + DIFICULDADE_DIFICIL_AJUSTE_TEMPO_LIMITE;
    return TEMPO_LIMITE + DIFICULDADE_NORMAL_AJUSTE_TEMPO_LIMITE;
}

int inimigo_ondas_vitoria(int dificuldade)
{
    int ondas_para_vencer;
    if (dificuldade <= 0)
        ondas_para_vencer = ONDAS_TOTAIS + DIFICULDADE_FACIL_AJUSTE_ONDAS_MAXIMAS;
    else if (dificuldade >= 2)
        ondas_para_vencer = ONDAS_TOTAIS + DIFICULDADE_DIFICIL_AJUSTE_ONDAS_MAXIMAS;
    else
        ondas_para_vencer = ONDAS_TOTAIS + DIFICULDADE_NORMAL_AJUSTE_ONDAS_MAXIMAS;
    if (ondas_para_vencer < 1)
        ondas_para_vencer = 1;
    return ondas_para_vencer;
}
