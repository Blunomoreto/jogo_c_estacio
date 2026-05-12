#include "matematica.h"
#include "tipos.h"

#include <math.h>
#include <stdlib.h>

float matematica_float_aleatorio(float minV, float maxV)
{
    return minV + (maxV - minV) * ((float)rand() / (float)RAND_MAX);
}

Vetor2D matematica_vetor2d(float x, float y)
{
    Vetor2D v;
    v.x = x;
    v.y = y;
    return v;
}

Vetor2D matematica_vetor2d_adicao(Vetor2D a, Vetor2D b)
{
    return matematica_vetor2d(a.x + b.x, a.y + b.y);
}

Vetor2D matematica_vetor2d_subtracao(Vetor2D a, Vetor2D b)
{
    return matematica_vetor2d(a.x - b.x, a.y - b.y);
}

Vetor2D matematica_vetor2d_multiplicacao(Vetor2D v, float s)
{
    return matematica_vetor2d(v.x * s, v.y * s);
}

float matematica_vetor2d_len(Vetor2D v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vetor2D matematica_vetor2d_normalizar(Vetor2D v)
{
    float len = matematica_vetor2d_len(v);
    if (len <= 0.0001f)
    {
        return matematica_vetor2d(0.0f, 1.0f);
    }
    return matematica_vetor2d(v.x / len, v.y / len);
}

Vetor2D matematica_mouse_para_mundo(Jogo *g)
{
    return matematica_vetor2d((float)g->entrada.mouseX, (float)g->entrada.mouseY);
}

float matematica_vetor2d_distance_sq(Vetor2D a, Vetor2D b)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

float matematica_limite_min_max(float a, float min, float max)
{
    const float clamped = a < min ? min : a;
    return clamped > max ? max : clamped;
}

float matematica_limite_min(float a, float min)
{
    const float clamped = a < min ? min : a;
    return clamped;
}

float matematica_limite_max(float a, float max)
{
    const float clamped = a > max ? max : a;
    return clamped;
}