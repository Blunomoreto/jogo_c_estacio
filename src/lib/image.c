#include "image.h"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

static int image_skip_comments(FILE *f)
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

unsigned int image_load_texture_ppm(const char *path, int *ok)
{
    FILE *f = fopen(path, "rb");
    char magic[3] = {0};
    int width = 0;
    int height = 0;
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

    if (!image_skip_comments(f) || fscanf(f, "%d", &width) != 1)
    {
        fclose(f);
        return 0;
    }
    if (!image_skip_comments(f) || fscanf(f, "%d", &height) != 1)
    {
        fclose(f);
        return 0;
    }
    if (!image_skip_comments(f) || fscanf(f, "%d", &maxval) != 1 || maxval != 255)
    {
        fclose(f);
        return 0;
    }
    fgetc(f);

    data = (unsigned char *)malloc((size_t)width * (size_t)height * 3);
    if (!data)
    {
        fclose(f);
        return 0;
    }

    if (magic[1] == '6')
    {
        if (fread(data, 1, (size_t)width * (size_t)height * 3, f) != (size_t)width * (size_t)height * 3)
        {
            free(data);
            fclose(f);
            return 0;
        }
    }
    else
    {
        size_t i;
        for (i = 0; i < (size_t)width * (size_t)height * 3; ++i)
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    *ok = 1;
    return tex;
}
