#include "matematica.h"
#include "tipos.h"

#include <math.h>
#include <stdlib.h>

Vetor2D matematica_vetor2d(float x, float y)
{
    Vetor2D vetor;
    vetor.x = x;
    vetor.y = y;
    return vetor;
}

Vetor2D matematica_vetor2d_adicao(Vetor2D vetor1, Vetor2D vetor2)
{
    return matematica_vetor2d(vetor1.x + vetor2.x, vetor1.y + vetor2.y);
}

Vetor2D matematica_vetor2d_subtracao(Vetor2D vetor1, Vetor2D vetor2)
{
    return matematica_vetor2d(vetor1.x - vetor2.x, vetor1.y - vetor2.y);
}

Vetor2D matematica_vetor2d_multiplicacao(Vetor2D vetor, float multiplicador)
{
    return matematica_vetor2d(vetor.x * multiplicador, vetor.y * multiplicador);
}

Vetor2D matematica_vetor2d_normalizar(Vetor2D vetor)
{
    float len = matematica_vetor2d_len(vetor);
    if (len <= 0.0001f)
    {
        return matematica_vetor2d(0.0f, 1.0f);
    }
    return matematica_vetor2d(vetor.x / len, vetor.y / len);
}

Vetor2D matematica_mouse_para_mundo(Game *jogo)
{
    return matematica_vetor2d((float)jogo->input.mouseX, (float)jogo->input.mouseY);
}

float matematica_vetor2d_len(Vetor2D vetor)
{
    return sqrtf(vetor.x * vetor.x + vetor.y * vetor.y);
}

float matematica_vetor2d_distance_sq(Vetor2D vetor1, Vetor2D vetor2)
{
    const float dx = vetor1.x - vetor2.x;
    const float dy = vetor1.y - vetor2.y;
    return dx * dx + dy * dy;
}

float matematica_float_aleatorio_alcance(float min, float max)
{
    return min + (max - min) * ((float)rand() / (float)RAND_MAX);
}

float matematica_limite_min_max(float valor, float min, float max)
{
    const float clamped = valor < min ? min : valor;
    return clamped > max ? max : clamped;
}

float matematica_limite_min(float valor, float min)
{
    const float clamped = valor < min ? min : valor;
    return clamped;
}

float matematica_limite_max(float valor, float max)
{
    const float clamped = valor > max ? max : valor;
    return clamped;
}
