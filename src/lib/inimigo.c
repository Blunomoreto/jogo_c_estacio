#include "inimigo.h"
#include "matematica.h"

#include <math.h>

Vetor2D inimigo_posicao(const Inimigo *e)
{
    return matematica_vetor2d(e->centro.x + cosf(e->angulo) * e->raioOrbita,
                              e->centro.y + sinf(e->angulo) * e->raioOrbita);
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
    int v;
    if (dificuldade <= 0)
        v = ONDAS_TOTAIS + DIFICULDADE_FACIL_AJUSTE_ONDAS_MAXIMAS;
    else if (dificuldade >= 2)
        v = ONDAS_TOTAIS + DIFICULDADE_DIFICIL_AJUSTE_ONDAS_MAXIMAS;
    else
        v = ONDAS_TOTAIS + DIFICULDADE_NORMAL_AJUSTE_ONDAS_MAXIMAS;
    if (v < 1)
        v = 1;
    return v;
}

float inimigo_multiplicador_pontos(int dificuldade)
{
    if (dificuldade <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (dificuldade >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}