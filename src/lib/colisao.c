#include "colisao.h"
#include "matematica.h"

#include <math.h>

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

static Vetor2D colisao_ponto_para_local_obb(Vetor2D ponto_mundo, Vetor2D centro_retangulo, float angulo_graus)
{
    float angulo_em_radianos = -angulo_graus * (float)M_PI / 180.0f;
    float cosseno_angulo = cosf(angulo_em_radianos);
    float seno_angulo = sinf(angulo_em_radianos);
    Vetor2D deslocamento = matematica_vetor2d_subtracao(ponto_mundo, centro_retangulo);
    Vetor2D ponto_em_coordenadas_locais;
    ponto_em_coordenadas_locais.x = deslocamento.x * cosseno_angulo - deslocamento.y * seno_angulo;
    ponto_em_coordenadas_locais.y = deslocamento.x * seno_angulo + deslocamento.y * cosseno_angulo;
    return ponto_em_coordenadas_locais;
}

static Vetor2D colisao_local_para_mundo_obb(Vetor2D ponto_local, Vetor2D centro_retangulo, float angulo_graus)
{
    float angulo_em_radianos = angulo_graus * (float)M_PI / 180.0f;
    float cosseno_angulo = cosf(angulo_em_radianos);
    float seno_angulo = sinf(angulo_em_radianos);
    Vetor2D ponto_em_coordenadas_mundo;
    ponto_em_coordenadas_mundo.x = ponto_local.x * cosseno_angulo - ponto_local.y * seno_angulo + centro_retangulo.x;
    ponto_em_coordenadas_mundo.y = ponto_local.x * seno_angulo + ponto_local.y * cosseno_angulo + centro_retangulo.y;
    return ponto_em_coordenadas_mundo;
}

Vetor2D colisao_resolver_circulo_vs_obb(Vetor2D centro_circulo, float raio, Vetor2D centro_retangulo, float largura, float altura, float angulo_graus, int *colidiu)
{
    Vetor2D ponto_em_coordenadas_locais = colisao_ponto_para_local_obb(centro_circulo, centro_retangulo, angulo_graus);
    float meia_largura = largura * 0.5f;
    float meia_altura = altura * 0.5f;
    float distancia_para_esquerda;
    float distancia_para_direita;
    float distancia_para_topo;
    float distancia_para_baixo;
    float distancia_minima_para_borda;
    Vetor2D vetor_normal_local;
    Vetor2D posicao_empurrada_local;
    Vetor2D nova_posicao_em_mundo;

    if (colidiu)
        *colidiu = 0;

    if (ponto_em_coordenadas_locais.x < -meia_largura - raio || ponto_em_coordenadas_locais.x > meia_largura + raio ||
        ponto_em_coordenadas_locais.y < -meia_altura - raio || ponto_em_coordenadas_locais.y > meia_altura + raio)
        return centro_circulo;

    if (ponto_em_coordenadas_locais.x >= -meia_largura && ponto_em_coordenadas_locais.x <= meia_largura &&
        ponto_em_coordenadas_locais.y >= -meia_altura && ponto_em_coordenadas_locais.y <= meia_altura)
    {
        distancia_para_esquerda = ponto_em_coordenadas_locais.x - (-meia_largura);
        distancia_para_direita = meia_largura - ponto_em_coordenadas_locais.x;
        distancia_para_topo = ponto_em_coordenadas_locais.y - (-meia_altura);
        distancia_para_baixo = meia_altura - ponto_em_coordenadas_locais.y;
        distancia_minima_para_borda = distancia_para_esquerda;
        vetor_normal_local = matematica_vetor2d(-1.0f, 0.0f);
        if (distancia_para_direita < distancia_minima_para_borda)
        {
            distancia_minima_para_borda = distancia_para_direita;
            vetor_normal_local = matematica_vetor2d(1.0f, 0.0f);
        }
        if (distancia_para_topo < distancia_minima_para_borda)
        {
            distancia_minima_para_borda = distancia_para_topo;
            vetor_normal_local = matematica_vetor2d(0.0f, -1.0f);
        }
        if (distancia_para_baixo < distancia_minima_para_borda)
        {
            distancia_minima_para_borda = distancia_para_baixo;
            vetor_normal_local = matematica_vetor2d(0.0f, 1.0f);
        }

        posicao_empurrada_local.x = ponto_em_coordenadas_locais.x + vetor_normal_local.x * (distancia_minima_para_borda + raio);
        posicao_empurrada_local.y = ponto_em_coordenadas_locais.y + vetor_normal_local.y * (distancia_minima_para_borda + raio);
        nova_posicao_em_mundo = colisao_local_para_mundo_obb(posicao_empurrada_local, centro_retangulo, angulo_graus);
        if (colidiu)
            *colidiu = 1;
        return nova_posicao_em_mundo;
    }

    {
        float ponto_mais_proximo_x = ponto_em_coordenadas_locais.x;
        float ponto_mais_proximo_y = ponto_em_coordenadas_locais.y;
        float diferenca_x;
        float diferenca_y;
        float distancia_ao_quadrado;
        float distancia_real;

        if (ponto_mais_proximo_x < -meia_largura)
            ponto_mais_proximo_x = -meia_largura;
        if (ponto_mais_proximo_x > meia_largura)
            ponto_mais_proximo_x = meia_largura;
        if (ponto_mais_proximo_y < -meia_altura)
            ponto_mais_proximo_y = -meia_altura;
        if (ponto_mais_proximo_y > meia_altura)
            ponto_mais_proximo_y = meia_altura;

        diferenca_x = ponto_em_coordenadas_locais.x - ponto_mais_proximo_x;
        diferenca_y = ponto_em_coordenadas_locais.y - ponto_mais_proximo_y;
        distancia_ao_quadrado = diferenca_x * diferenca_x + diferenca_y * diferenca_y;
        if (distancia_ao_quadrado > raio * raio)
            return centro_circulo;

        distancia_real = sqrtf(distancia_ao_quadrado);
        if (distancia_real < 0.0001f)
            return centro_circulo;

        posicao_empurrada_local.x = ponto_mais_proximo_x + (diferenca_x / distancia_real) * raio;
        posicao_empurrada_local.y = ponto_mais_proximo_y + (diferenca_y / distancia_real) * raio;
        nova_posicao_em_mundo = colisao_local_para_mundo_obb(posicao_empurrada_local, centro_retangulo, angulo_graus);
        if (colidiu)
            *colidiu = 1;
        return nova_posicao_em_mundo;
    }
}
