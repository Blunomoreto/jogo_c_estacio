#include "imagem.h"

#include <GL/glut.h>
#include <math.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>

static int imagem_pular_comentarios(FILE *f)
{
    int c = fgetc(f);
    while (c == '#')
    {
        while (c != '\n' && c != EOF)
        {
            c = fgetc(f);
        }
        c = fgetc(f);
    }
    if (c != EOF)
    {
        ungetc(c, f);
    }
    return c != EOF;
}

unsigned int imagem_carregar_ppm(const char *caminho_arquivo, int *ok)
{
    FILE *f = fopen(caminho_arquivo, "rb");
    char magic[3] = {0};
    int largura = 0;
    int altura = 0;
    int maxval = 0;
    unsigned char *data;
    GLuint tex = 0;

    *ok = 0;
    if (!f)
    {
        return 0;
    }

    if (fscanf(f, "%2s", magic) != 1 || magic[0] != 'P' || (magic[1] != '6' && magic[1] != '3'))
    {
        fclose(f);
        return 0;
    }

    if (!imagem_pular_comentarios(f) || fscanf(f, "%d", &largura) != 1)
    {
        fclose(f);
        return 0;
    }
    if (!imagem_pular_comentarios(f) || fscanf(f, "%d", &altura) != 1)
    {
        fclose(f);
        return 0;
    }
    if (!imagem_pular_comentarios(f) || fscanf(f, "%d", &maxval) != 1 || maxval != 255)
    {
        fclose(f);
        return 0;
    }
    fgetc(f);

    data = (unsigned char *)malloc((size_t)largura * (size_t)altura * 3);
    if (!data)
    {
        fclose(f);
        return 0;
    }

    if (magic[1] == '6')
    {
        if (fread(data, 1, (size_t)largura * (size_t)altura * 3, f) != (size_t)largura * (size_t)altura * 3)
        {
            free(data);
            fclose(f);
            return 0;
        }
    }
    else
    {
        size_t i;
        for (i = 0; i < (size_t)largura * (size_t)altura * 3; ++i)
        {
            int v = 0;
            if (fscanf(f, "%d", &v) != 1)
            {
                free(data);
                fclose(f);
                return 0;
            }
            if (v < 0)
                v = 0;
            if (v > 255)
                v = 255;
            data[i] = (unsigned char)v;
        }
    }
    fclose(f);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    *ok = 1;
    return tex;
}

static unsigned int lcg_rand(unsigned int *estado)
{
    *estado = *estado * 1664525u + 1013904223u;
    return *estado;
}

void imagem_gerar_fundo_estelar(const char *caminho, int largura, int altura)
{
    int total = largura * altura * 3;
    unsigned char *pixels = (unsigned char *)malloc((size_t)total);
    int x, y, i;
    unsigned int rng = 0x5A3C1B2Eu;

    if (!pixels)
        return;

    for (y = 0; y < altura; y++)
    {
        for (x = 0; x < largura; x++)
        {
            int idx = (y * largura + x) * 3;
            float ty = (float)y / (float)(altura > 1 ? altura - 1 : 1);
            pixels[idx + 0] = (unsigned char)(8 - (int)(ty * 5.0f));
            pixels[idx + 1] = (unsigned char)(12 - (int)(ty * 6.0f));
            pixels[idx + 2] = (unsigned char)(28 - (int)(ty * 16.0f));
        }
    }

    for (i = 0; i < 320; i++)
    {
        int sx = (int)(lcg_rand(&rng) % (unsigned int)largura);
        int sy = (int)(lcg_rand(&rng) % (unsigned int)(altura * 3 / 4));
        unsigned char brilho = (unsigned char)(160 + lcg_rand(&rng) % 96);
        int idx = (sy * largura + sx) * 3;
        pixels[idx + 0] = brilho;
        pixels[idx + 1] = brilho;
        pixels[idx + 2] = (unsigned char)255;
    }

    {
        int mx = largura * 85 / 100;
        int my = altura * 22 / 100;
        float raio = (float)largura * 0.09f;
        for (y = 0; y < altura; y++)
        {
            for (x = 0; x < largura; x++)
            {
                float dx = (float)(x - mx);
                float dy = (float)(y - my);
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < raio)
                {
                    int idx = (y * largura + x) * 3;
                    float fac = 1.0f - dist / raio;
                    pixels[idx + 0] = (unsigned char)(pixels[idx + 0] + (int)(fac * (242.0f - pixels[idx + 0])));
                    pixels[idx + 1] = (unsigned char)(pixels[idx + 1] + (int)(fac * (242.0f - pixels[idx + 1])));
                    pixels[idx + 2] = (unsigned char)(pixels[idx + 2] + (int)(fac * (230.0f - pixels[idx + 2])));
                }
            }
        }
        {
            int ox = mx + (int)(raio * 0.38f);
            int oy = my - (int)(raio * 0.18f);
            float raio_sombra = raio * 0.92f;
            for (y = 0; y < altura; y++)
            {
                for (x = 0; x < largura; x++)
                {
                    float dx = (float)(x - ox);
                    float dy = (float)(y - oy);
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < raio_sombra)
                    {
                        int idx = (y * largura + x) * 3;
                        float fac = 1.0f - dist / raio_sombra;
                        pixels[idx + 0] = (unsigned char)(pixels[idx + 0] * (1.0f - fac * 0.95f));
                        pixels[idx + 1] = (unsigned char)(pixels[idx + 1] * (1.0f - fac * 0.95f));
                        pixels[idx + 2] = (unsigned char)(pixels[idx + 2] * (1.0f - fac * 0.55f));
                    }
                }
            }
        }
    }

    stbi_write_png(caminho, largura, altura, 3, pixels, largura * 3);
    free(pixels);
}

unsigned int imagem_carregar_stb(const char *caminho, int *ok)
{
    int largura, altura, canais;
    unsigned char *dados = stbi_load(caminho, &largura, &altura, &canais, 3);
    GLuint tex = 0;

    *ok = 0;
    if (!dados)
        return 0;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
    *ok = 1;
    return tex;
}

unsigned int imagem_carregar_fundo(int *ok)
{
    const char *caminho_png = "assets/images/background.png";
    FILE *teste = fopen(caminho_png, "rb");

    if (!teste)
        imagem_gerar_fundo_estelar(caminho_png, 512, 512);
    else
        fclose(teste);

    return imagem_carregar_stb(caminho_png, ok);
}

unsigned int imagem_salvar_png(const char *caminho, int largura, int altura)
{
    const size_t bytes = (size_t)largura * (size_t)altura * 3;
    unsigned char *pixels = (unsigned char *)malloc(bytes);
    unsigned char *flipped = (unsigned char *)malloc(bytes);
    int y, resultado;

    if (!pixels || !flipped)
    {
        free(pixels);
        free(flipped);
        return 0;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, largura, altura, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    for (y = 0; y < altura; ++y)
    {
        const size_t src = (size_t)y * (size_t)largura * 3;
        const size_t dst = (size_t)(altura - 1 - y) * (size_t)largura * 3;
        size_t i;
        for (i = 0; i < (size_t)largura * 3; ++i)
            flipped[dst + i] = pixels[src + i];
    }

    resultado = stbi_write_png(caminho, largura, altura, 3, flipped, largura * 3);
    free(pixels);
    free(flipped);
    return resultado ? 1 : 0;
}

unsigned int imagem_salvar_ppm(const char *nome_arquivo, int largura, int altura)
{
    const size_t bytes = (size_t)largura * (size_t)altura * 3;
    unsigned char *pixels = (unsigned char *)malloc(bytes);
    unsigned char *flipped = (unsigned char *)malloc(bytes);
    int y;

    if (!pixels || !flipped)
    {
        free(pixels);
        free(flipped);
        return 0;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, largura, altura, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    for (y = 0; y < altura; ++y)
    {
        const size_t src = (size_t)y * (size_t)largura * 3;
        const size_t dst = (size_t)(altura - 1 - y) * (size_t)largura * 3;
        size_t i;
        for (i = 0; i < (size_t)largura * 3; ++i)
        {
            flipped[dst + i] = pixels[src + i];
        }
    }

    {
        FILE *f = fopen(nome_arquivo, "wb");
        if (!f)
        {
            free(pixels);
            free(flipped);
            return 0;
        }
        fprintf(f, "P6\n%d %d\n255\n", largura, altura);
        fwrite(flipped, 1, bytes, f);
        fclose(f);
    }

    free(pixels);
    free(flipped);
    return 1;
}
