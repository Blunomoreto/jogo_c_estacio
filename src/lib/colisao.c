#include "colisao.h"
#include "matematica.h"

int colisao_circulo_vs_circulo(Vetor2D posicaoA, float raioA, Vetor2D posicaoB, float raioB)
{
    const float raio = raioA + raioB;

    return matematica_vetor2d_distance_sq(posicaoA, posicaoB) <= raio * raio;
}

int colisao_circulo_vs_retangulo(Vetor2D posicaoA, float raioA, Vetor2D cantoSuperiorEsquerdo, Vetor2D cantoSuperiorDireito)
{
    float closestX = posicaoA.x;
    float closestY = posicaoA.y;
    float dx;
    float dy;

    if (closestX < cantoSuperiorEsquerdo.x)
        closestX = cantoSuperiorEsquerdo.x;

    if (closestX > cantoSuperiorDireito.x)
        closestX = cantoSuperiorDireito.x;

    if (closestY < cantoSuperiorEsquerdo.y)
        closestY = cantoSuperiorEsquerdo.y;

    if (closestY > cantoSuperiorDireito.y)
        closestY = cantoSuperiorDireito.y;

    dx = posicaoA.x - closestX;
    dy = posicaoA.y - closestY;

    return (dx * dx + dy * dy) <= raioA * raioA;
}
