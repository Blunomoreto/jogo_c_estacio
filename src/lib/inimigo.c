#include "inimigo.h"
#include "matematica.h"

#include <math.h>

Vetor2D inimigo_posicao(const Enemy *e)
{
    return matematica_vetor2d(e->center.x + cosf(e->angle) * e->orbitRadius,
                      e->center.y + sinf(e->angle) * e->orbitRadius);
}

Vetor2D inimigo_velocidade(const Enemy *e)
{
    return matematica_vetor2d(-e->orbitRadius * e->angularSpeed * sinf(e->angle),
                      e->orbitRadius * e->angularSpeed * cosf(e->angle));
}

Vetor2D inimigo_aceleracao(const Enemy *e)
{
    float omega = e->angularSpeed;
    return matematica_vetor2d(-e->orbitRadius * omega * omega * cosf(e->angle),
                      -e->orbitRadius * omega * omega * sinf(e->angle));
}

const char *inimigo_nome_dificuldade(int difficulty)
{
    if (difficulty <= 0)
        return "EASY";
    if (difficulty >= 2)
        return "HARD";
    return "NORMAL";
}

float inimigo_multiplicador_vida(int difficulty)
{
    if (difficulty <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (difficulty >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_multiplicador_dano(int difficulty)
{
    if (difficulty <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (difficulty >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_taxa_disparo(int difficulty)
{
    if (difficulty <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (difficulty >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}

float inimigo_tempo_inicio(int difficulty)
{
    if (difficulty <= 0)
        return TEMPO_LIMITE + DIFICULDADE_FACIL_AJUSTE_TEMPO_LIMITE;
    if (difficulty >= 2)
        return TEMPO_LIMITE + DIFICULDADE_DIFICIL_AJUSTE_TEMPO_LIMITE;
    return TEMPO_LIMITE + DIFICULDADE_NORMAL_AJUSTE_TEMPO_LIMITE;
}

int inimigo_ondas_vitoria(int difficulty)
{
    int v;
    if (difficulty <= 0)
        v = ONDAS_TOTAIS + DIFICULDADE_FACIL_AJUSTE_ONDAS_MAXIMAS;
    else if (difficulty >= 2)
        v = ONDAS_TOTAIS + DIFICULDADE_DIFICIL_AJUSTE_ONDAS_MAXIMAS;
    else
        v = ONDAS_TOTAIS + DIFICULDADE_NORMAL_AJUSTE_ONDAS_MAXIMAS;
    if (v < 1)
        v = 1;
    return v;
}

float inimigo_multiplicador_pontos(int difficulty)
{
    if (difficulty <= 0)
        return DIFICULDADE_FACIL_MULTIPLICADOR;
    if (difficulty >= 2)
        return DIFICULDADE_DIFICIL_MULTIPLICADOR;
    return DIFICULDADE_NORMAL_MULTIPLICADOR;
}
