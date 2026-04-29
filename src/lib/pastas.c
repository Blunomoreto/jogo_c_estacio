#include "configuracao.h"
#include "pastas.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat status = {0};

void pastas_criar(void)
{
    if (stat(PASTA_ASSETS, &status) == -1)
    {
        mkdir(PASTA_ASSETS);
        mkdir(PASTA_DATA);
        mkdir(PASTA_IMAGES);
        mkdir(PASTA_SCREENSHOTS);
        return;
    }

    if (stat(PASTA_DATA, &status) == -1)
    {
        mkdir(PASTA_DATA);
    }

    if (stat(PASTA_IMAGES, &status) == -1)
    {
        mkdir(PASTA_IMAGES);
    }

    if (stat(PASTA_SCREENSHOTS, &status) == -1)
    {
        mkdir(PASTA_SCREENSHOTS);
    }
}
