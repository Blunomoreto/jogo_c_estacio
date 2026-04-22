#include "persistence.h"
#include "config.h"

#include <stdio.h>
#include <string.h>

void persistence_load_stats(int *highScore, int *maxWave)
{
    FILE *f = fopen(STATS_FILE, "r");
    if (!f)
    {
        *highScore = 0;
        *maxWave = 0;
        return;
    }

    if (fscanf(f, "%d %d", highScore, maxWave) != 2)
    {
        *highScore = 0;
        *maxWave = 0;
    }
    fclose(f);
}

void persistence_save_stats(int highScore, int maxWave)
{
    FILE *f = fopen(STATS_FILE, "w");
    if (!f)
    {
        return;
    }
    fprintf(f, "%d %d\n", highScore, maxWave);
    fclose(f);
}

void persistence_append_score(const char *name, int score, int wave)
{
    FILE *f = fopen(DATA_FILE, "a");
    if (!f)
    {
        return;
    }
    fprintf(f, "%s;%d;%d\n", name, score, wave);
    fclose(f);
}

int persistence_load_top_scores(ScoreEntry *outEntries, int maxEntries)
{
    FILE *f = fopen(DATA_FILE, "r");
    char line[160];
    int count = 0;

    if (!outEntries || maxEntries <= 0)
    {
        return 0;
    }

    memset(outEntries, 0, sizeof(ScoreEntry) * (size_t)maxEntries);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        ScoreEntry entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.name, &entry.score, &entry.wave) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.score > outEntries[i].score)
            {
                int j;
                int limit = (count < maxEntries) ? count : maxEntries - 1;
                for (j = limit; j > i; --j)
                {
                    outEntries[j] = outEntries[j - 1];
                }
                outEntries[i] = entry;
                if (count < maxEntries)
                {
                    count++;
                }
                inserted = 1;
                break;
            }
        }

        if (!inserted && count < maxEntries)
        {
            outEntries[count++] = entry;
        }
    }

    fclose(f);
    return count;
}

int persistence_load_scores(ScoreEntry *outEntries, int maxEntries)
{
    FILE *f = fopen(DATA_FILE, "r");
    char line[160];
    int count = 0;

    if (!outEntries || maxEntries <= 0)
    {
        return 0;
    }

    memset(outEntries, 0, sizeof(ScoreEntry) * (size_t)maxEntries);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        ScoreEntry entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.name, &entry.score, &entry.wave) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.score > outEntries[i].score)
            {
                int j;
                int limit = (count < maxEntries) ? count : maxEntries - 1;
                for (j = limit; j > i; --j)
                {
                    outEntries[j] = outEntries[j - 1];
                }
                outEntries[i] = entry;
                if (count < maxEntries)
                {
                    count++;
                }
                inserted = 1;
                break;
            }
        }

        if (!inserted && count < maxEntries)
        {
            outEntries[count++] = entry;
        }
    }

    fclose(f);
    return count;
}

void persistence_clear_scores(void)
{
    FILE *f = fopen(DATA_FILE, "w");
    if (!f)
    {
        return;
    }
    fclose(f);
}

void persistence_load_settings(int *audioEnabled, int *difficulty)
{
    FILE *f = fopen(SETTINGS_FILE, "r");
    int a = 1;
    int d = 1;

    if (f)
    {
        if (fscanf(f, "%d %d", &a, &d) != 2)
        {
            a = 1;
            d = 1;
        }
        fclose(f);
    }

    if (d < 0)
        d = 0;
    if (d > 2)
        d = 2;

    if (audioEnabled)
    {
        *audioEnabled = a ? 1 : 0;
    }
    if (difficulty)
    {
        *difficulty = d;
    }
}

void persistence_save_settings(int audioEnabled, int difficulty)
{
    FILE *f = fopen(SETTINGS_FILE, "w");
    if (!f)
    {
        return;
    }
    if (difficulty < 0)
        difficulty = 0;
    if (difficulty > 2)
        difficulty = 2;
    fprintf(f, "%d %d\n", audioEnabled ? 1 : 0, difficulty);
    fclose(f);
}
