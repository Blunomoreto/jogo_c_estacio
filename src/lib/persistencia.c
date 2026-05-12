#include "persistencia.h"
#include "configuracao.h"

#include <stdio.h>
#include <string.h>

void persistencia_carregar_stats(int *pontuacao_maxima, int *onda_maxima)
{
    FILE *f = fopen(ARQUIVO_STATS, "r");
    if (!f)
    {
        *pontuacao_maxima = 0;
        *onda_maxima = 0;
        return;
    }

    if (fscanf(f, "%d %d", pontuacao_maxima, onda_maxima) != 2)
    {
        *pontuacao_maxima = 0;
        *onda_maxima = 0;
    }
    fclose(f);
}

void persistencia_salvar_stats(int pontuacao_maxima, int onda_maxima)
{
    FILE *f = fopen(ARQUIVO_STATS, "w");
    if (!f)
    {
        return;
    }
    fprintf(f, "%d %d\n", pontuacao_maxima, onda_maxima);
    fclose(f);
}

void persistencia_apor_pontuacao(const char *nome, int pontuacao, int onda)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "a");
    if (!f)
    {
        return;
    }
    fprintf(f, "%s;%d;%d\n", nome, pontuacao, onda);
    fclose(f);
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

void persistencia_carregar_configuracoes(int *audio_ativado, int *dificuldade)
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

    if (audio_ativado)
    {
        *audio_ativado = a ? 1 : 0;
    }
    if (dificuldade)
    {
        *dificuldade = d;
    }
}

void persistencia_salvar_configuracoes(int audio_ativado, int dificuldade)
{
    FILE *f = fopen(ARQUIVO_SETTINGS, "w");
    if (!f)
    {
        return;
    }
    if (dificuldade < 0)
        dificuldade = 0;
    if (dificuldade > 2)
        dificuldade = 2;
    fprintf(f, "%d %d\n", audio_ativado ? 1 : 0, dificuldade);
    fclose(f);
}

int persistencia_carregar_pontuacoes_altas(RegistroPontuacao *entradas_fora_lista, int entradas_maximas)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "r");
    char line[160];
    int count = 0;

    if (!entradas_fora_lista || entradas_maximas <= 0)
    {
        return 0;
    }

    memset(entradas_fora_lista, 0, sizeof(RegistroPontuacao) * (size_t)entradas_maximas);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        RegistroPontuacao entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.nome, &entry.pontuacao, &entry.onda) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.pontuacao > entradas_fora_lista[i].pontuacao)
            {
                int j;
                int limit = (count < entradas_maximas) ? count : entradas_maximas - 1;
                for (j = limit; j > i; --j)
                {
                    entradas_fora_lista[j] = entradas_fora_lista[j - 1];
                }
                entradas_fora_lista[i] = entry;
                if (count < entradas_maximas)
                {
                    count++;
                }
                inserted = 1;
                break;
            }
        }

        if (!inserted && count < entradas_maximas)
        {
            entradas_fora_lista[count++] = entry;
        }
    }

    fclose(f);
    return count;
}

int persistencia_carregar_pontuacoes(RegistroPontuacao *entradas_fora_lista, int entradas_maximas)
{
    FILE *f = fopen(ARQUIVO_SCOREBOARD, "r");
    char line[160];
    int count = 0;

    if (!entradas_fora_lista || entradas_maximas <= 0)
    {
        return 0;
    }

    memset(entradas_fora_lista, 0, sizeof(RegistroPontuacao) * (size_t)entradas_maximas);

    if (!f)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), f))
    {
        RegistroPontuacao entry;
        int inserted = 0;
        int i;

        memset(&entry, 0, sizeof(entry));
        if (sscanf(line, "%23[^;];%d;%d", entry.nome, &entry.pontuacao, &entry.onda) != 3)
        {
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (entry.pontuacao > entradas_fora_lista[i].pontuacao)
            {
                int j;
                int limit = (count < entradas_maximas) ? count : entradas_maximas - 1;
                for (j = limit; j > i; --j)
                {
                    entradas_fora_lista[j] = entradas_fora_lista[j - 1];
                }
                entradas_fora_lista[i] = entry;
                if (count < entradas_maximas)
                {
                    count++;
                }
                inserted = 1;
                break;
            }
        }

        if (!inserted && count < entradas_maximas)
        {
            entradas_fora_lista[count++] = entry;
        }
    }

    fclose(f);
    return count;
}
