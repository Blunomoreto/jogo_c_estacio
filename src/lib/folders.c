#include "config.h"
#include "folders.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat status = {0};

void folders_setup(void)
{
    if (stat(ASSETS_FOLDER, &status) == -1)
    {
        mkdir(ASSETS_FOLDER);
        mkdir(DATA_FOLDER);
        mkdir(IMAGES_FOLDER);
        mkdir(SCREENSHOTS_FOLDER);
        return;
    }

    if (stat(DATA_FOLDER, &status) == -1)
    {
        mkdir(DATA_FOLDER);
    }

    if (stat(IMAGES_FOLDER, &status) == -1)
    {
        mkdir(IMAGES_FOLDER);
    }

    if (stat(SCREENSHOTS_FOLDER, &status) == -1)
    {
        mkdir(SCREENSHOTS_FOLDER);
    }
}
