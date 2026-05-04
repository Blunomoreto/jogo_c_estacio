#include "imagem.h"

#include <GL/glut.h>
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
