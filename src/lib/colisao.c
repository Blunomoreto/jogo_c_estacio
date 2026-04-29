#include "colisao.h"
#include "matematica.h"

int colisao_circulo_vs_circulo(Vetor2D a, float ra, Vetor2D b, float rb)
{
    const float r = ra + rb;
    return matematica_vetor2d_distance_sq(a, b) <= r * r;
}

int colisao_circulo_vs_retangulo(Vetor2D c, float r, Vetor2D bmin, Vetor2D bmax)
{
    float closestX = c.x;
    float closestY = c.y;
    float dx;
    float dy;

    if (closestX < bmin.x)
        closestX = bmin.x;
    if (closestX > bmax.x)
        closestX = bmax.x;
    if (closestY < bmin.y)
        closestY = bmin.y;
    if (closestY > bmax.y)
        closestY = bmax.y;

    dx = c.x - closestX;
    dy = c.y - closestY;
    return (dx * dx + dy * dy) <= r * r;
}

