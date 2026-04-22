#include "setup_folders.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat status = {0};

void setup_folders(void)
{
    if (stat("assets/", &status) == -1)
    {
        mkdir("assets/");
        mkdir("assets/data/");
        mkdir("assets/images/");
        mkdir("assets/screenshots/");
        return;
    }

    if (stat("assets/data/", &status) == -1)
    {
        mkdir("assets/data/");
    }

    if (stat("assets/images/", &status) == -1)
    {
        mkdir("assets/images/");
    }

    if (stat("assets/screenshots/", &status) == -1)
    {
        mkdir("assets/screenshots/");
    }
}
