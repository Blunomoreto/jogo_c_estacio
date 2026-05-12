#include "persistencia.h"
#include "configuracao.h"

#include <stdio.h>
#include <string.h>

void persistencia_carregar_stats(int *pontuacao_maxima, int *onda_maxima)
{
    FILE *arquivo = fopen(ARQUIVO_STATS, "r");
    if (!arquivo)
    {
        *pontuacao_maxima = 0;
        *onda_maxima = 0;
        return;
    }

    if (fscanf(arquivo, "%d %d", pontuacao_maxima, onda_maxima) != 2)
    {
        *pontuacao_maxima = 0;
        *onda_maxima = 0;
    }
    fclose(arquivo);
}

void persistencia_salvar_stats(int pontuacao_maxima, int onda_maxima)
{
    FILE *arquivo = fopen(ARQUIVO_STATS, "w");
    if (!arquivo)
    {
        return;
    }
    fprintf(arquivo, "%d %d\n", pontuacao_maxima, onda_maxima);
    fclose(arquivo);
}

void persistencia_apor_pontuacao(const char *nome, int pontuacao, int onda)
{
    FILE *arquivo = fopen(ARQUIVO_SCOREBOARD, "a");
    if (!arquivo)
    {
        return;
    }
    fprintf(arquivo, "%s;%d;%d\n", nome, pontuacao, onda);
    fclose(arquivo);
}

void persistencia_limpar_pontuacoes(void)
{
    FILE *arquivo = fopen(ARQUIVO_SCOREBOARD, "w");
    if (!arquivo)
    {
        return;
    }
    fclose(arquivo);
}

void persistencia_carregar_configuracoes(int *audio_ativado, int *dificuldade)
{
    FILE *arquivo = fopen(ARQUIVO_SETTINGS, "r");
    int valor_audio = 1;
    int valor_dificuldade = 1;

    if (arquivo)
    {
        if (fscanf(arquivo, "%d %d", &valor_audio, &valor_dificuldade) != 2)
        {
            valor_audio = 1;
            valor_dificuldade = 1;
        }
        fclose(arquivo);
    }

    if (valor_dificuldade < 0)
        valor_dificuldade = 0;
    if (valor_dificuldade > 2)
        valor_dificuldade = 2;

    if (audio_ativado)
    {
        *audio_ativado = valor_audio ? 1 : 0;
    }
    if (dificuldade)
    {
        *dificuldade = valor_dificuldade;
    }
}

void persistencia_salvar_configuracoes(int audio_ativado, int dificuldade)
{
    FILE *arquivo = fopen(ARQUIVO_SETTINGS, "w");
    if (!arquivo)
    {
        return;
    }
    if (dificuldade < 0)
        dificuldade = 0;
    if (dificuldade > 2)
        dificuldade = 2;
    fprintf(arquivo, "%d %d\n", audio_ativado ? 1 : 0, dificuldade);
    fclose(arquivo);
}

int persistencia_carregar_pontuacoes_altas(RegistroPontuacao *entradas_fora_lista, int entradas_maximas)
{
    FILE *arquivo = fopen(ARQUIVO_SCOREBOARD, "r");
    char linha_lida[160];
    int quantidade_carregada = 0;

    if (!entradas_fora_lista || entradas_maximas <= 0)
    {
        return 0;
    }

    memset(entradas_fora_lista, 0, sizeof(RegistroPontuacao) * (size_t)entradas_maximas);

    if (!arquivo)
    {
        return 0;
    }

    while (fgets(linha_lida, sizeof(linha_lida), arquivo))
    {
        RegistroPontuacao registro;
        int foi_inserido = 0;
        int indice_entrada;

        memset(&registro, 0, sizeof(registro));
        if (sscanf(linha_lida, "%23[^;];%d;%d", registro.nome, &registro.pontuacao, &registro.onda) != 3)
        {
            continue;
        }

        for (indice_entrada = 0; indice_entrada < quantidade_carregada; ++indice_entrada)
        {
            if (registro.pontuacao > entradas_fora_lista[indice_entrada].pontuacao)
            {
                int indice_deslocamento;
                int limite_deslocamento = (quantidade_carregada < entradas_maximas) ? quantidade_carregada : entradas_maximas - 1;
                for (indice_deslocamento = limite_deslocamento; indice_deslocamento > indice_entrada; --indice_deslocamento)
                {
                    entradas_fora_lista[indice_deslocamento] = entradas_fora_lista[indice_deslocamento - 1];
                }
                entradas_fora_lista[indice_entrada] = registro;
                if (quantidade_carregada < entradas_maximas)
                {
                    quantidade_carregada++;
                }
                foi_inserido = 1;
                break;
            }
        }

        if (!foi_inserido && quantidade_carregada < entradas_maximas)
        {
            entradas_fora_lista[quantidade_carregada++] = registro;
        }
    }

    fclose(arquivo);
    return quantidade_carregada;
}

int persistencia_carregar_pontuacoes(RegistroPontuacao *entradas_fora_lista, int entradas_maximas)
{
    FILE *arquivo = fopen(ARQUIVO_SCOREBOARD, "r");
    char linha_lida[160];
    int quantidade_carregada = 0;

    if (!entradas_fora_lista || entradas_maximas <= 0)
    {
        return 0;
    }

    memset(entradas_fora_lista, 0, sizeof(RegistroPontuacao) * (size_t)entradas_maximas);

    if (!arquivo)
    {
        return 0;
    }

    while (fgets(linha_lida, sizeof(linha_lida), arquivo))
    {
        RegistroPontuacao registro;
        int foi_inserido = 0;
        int indice_entrada;

        memset(&registro, 0, sizeof(registro));
        if (sscanf(linha_lida, "%23[^;];%d;%d", registro.nome, &registro.pontuacao, &registro.onda) != 3)
        {
            continue;
        }

        for (indice_entrada = 0; indice_entrada < quantidade_carregada; ++indice_entrada)
        {
            if (registro.pontuacao > entradas_fora_lista[indice_entrada].pontuacao)
            {
                int indice_deslocamento;
                int limite_deslocamento = (quantidade_carregada < entradas_maximas) ? quantidade_carregada : entradas_maximas - 1;
                for (indice_deslocamento = limite_deslocamento; indice_deslocamento > indice_entrada; --indice_deslocamento)
                {
                    entradas_fora_lista[indice_deslocamento] = entradas_fora_lista[indice_deslocamento - 1];
                }
                entradas_fora_lista[indice_entrada] = registro;
                if (quantidade_carregada < entradas_maximas)
                {
                    quantidade_carregada++;
                }
                foi_inserido = 1;
                break;
            }
        }

        if (!foi_inserido && quantidade_carregada < entradas_maximas)
        {
            entradas_fora_lista[quantidade_carregada++] = registro;
        }
    }

    fclose(arquivo);
    return quantidade_carregada;
}
