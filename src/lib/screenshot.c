#include "screenshot.h"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

int screenshot_capture_ppm(const char *filename, int width, int height)
{
    const size_t bytes = (size_t)width * (size_t)height * 3;
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
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    for (y = 0; y < height; ++y)
    {
        const size_t src = (size_t)y * (size_t)width * 3;
        const size_t dst = (size_t)(height - 1 - y) * (size_t)width * 3;
        size_t i;
        for (i = 0; i < (size_t)width * 3; ++i)
        {
            flipped[dst + i] = pixels[src + i];
        }
    }

    {
        FILE *f = fopen(filename, "wb");
        if (!f)
        {
            free(pixels);
            free(flipped);
            return 0;
        }
        fprintf(f, "P6\n%d %d\n255\n", width, height);
        fwrite(flipped, 1, bytes, f);
        fclose(f);
    }

    free(pixels);
    free(flipped);
    return 1;
}
