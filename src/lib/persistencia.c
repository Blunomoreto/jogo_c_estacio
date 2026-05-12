#include "persistencia.h"
#include "configuracao.h"

#include <stdio.h>
#include <string.h>

void persistencia_carregar_stats(int *highScore, int *maxWave)
{
    FILE *f = fopen(ARQUIVO_STATS, "r");
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

void persistencia_salvar_stats(int highScore, int maxWave)
{
    FILE *f = fopen(ARQUIVO_STATS, "w");
    if (!f)
    {
        return;
    }
    fprintf(f, "%d %d\n", highScore, maxWave);
    fclose(f);
}

void persistencia_apor_pontuacao(const char *name, int score, int wave)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "a");
    if (!f)
    {
        return;
    }
    fprintf(f, "%s;%d;%d\n", name, score, wave);
    fclose(f);
}

int persistencia_carregar_pontuacoes_altas(EntradaPontuacao *outEntries, int maxEntries)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "r");
    char line[160];
    int count = 0;

    if (!outEntries || maxEntries <= 0)
    {
        return 0;
    }

    memset(outEntries, 0, sizeof(EntradaPontuacao) * (size_t)maxEntries);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        EntradaPontuacao entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.nome, &entry.pontuacao, &entry.onda) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.pontuacao > outEntries[i].pontuacao)
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

int persistencia_carregar_pontuacoes(EntradaPontuacao *outEntries, int maxEntries)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "r");
    char line[160];
    int count = 0;

    if (!outEntries || maxEntries <= 0)
    {
        return 0;
    }

    memset(outEntries, 0, sizeof(EntradaPontuacao) * (size_t)maxEntries);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        EntradaPontuacao entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.nome, &entry.pontuacao, &entry.onda) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.pontuacao > outEntries[i].pontuacao)
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

void persistencia_limpar_pontuacoes(void)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "w");
    if (!f)
    {
        return;
    }
    fclose(f);
}

void persistencia_carregar_configuracoes(int *audioEnabled, int *difficulty)
{
    FILE *f = fopen(ARQUIVO_SETTINGS, "r");
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

void persistencia_salvar_configuracoes(int audioEnabled, int difficulty)
{
    FILE *f = fopen(ARQUIVO_SETTINGS, "w");
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