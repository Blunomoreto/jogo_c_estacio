#include "audio.h"
#include "configuracao.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "winmm.lib")
#endif

static int s_audioAtivado = 1;

static unsigned char s_buf_bgm[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_BGM)];
static unsigned char s_buf_tiro[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_TIRO)];
static unsigned char s_buf_impacto[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_IMPACTO)];
static unsigned char s_buf_pulo[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_PULO)];
static unsigned char s_buf_aterrissagem[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_ATERR)];

static void escrever_cabecalho_wav(unsigned char *buf, int num_amostras)
{
    int tamanho_dados = num_amostras * AUDIO_BYTES_POR_AMOSTRA;
    int tamanho_chunk = 36 + tamanho_dados;
    int bytes_por_seg = AUDIO_TAXA_AMOSTRA * AUDIO_BYTES_POR_AMOSTRA;
    int alinhamento_bloco = AUDIO_BYTES_POR_AMOSTRA;

    buf[0] = 'R';
    buf[1] = 'I';
    buf[2] = 'F';
    buf[3] = 'F';
    buf[4] = (unsigned char)(tamanho_chunk & 0xFF);
    buf[5] = (unsigned char)((tamanho_chunk >> 8) & 0xFF);
    buf[6] = (unsigned char)((tamanho_chunk >> 16) & 0xFF);
    buf[7] = (unsigned char)((tamanho_chunk >> 24) & 0xFF);
    buf[8] = 'W';
    buf[9] = 'A';
    buf[10] = 'V';
    buf[11] = 'E';
    buf[12] = 'f';
    buf[13] = 'm';
    buf[14] = 't';
    buf[15] = ' ';
    buf[16] = 16;
    buf[17] = 0;
    buf[18] = 0;
    buf[19] = 0;
    buf[20] = 1;
    buf[21] = 0;
    buf[22] = 1;
    buf[23] = 0;
    buf[24] = (unsigned char)(AUDIO_TAXA_AMOSTRA & 0xFF);
    buf[25] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 8) & 0xFF);
    buf[26] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 16) & 0xFF);
    buf[27] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 24) & 0xFF);
    buf[28] = (unsigned char)(bytes_por_seg & 0xFF);
    buf[29] = (unsigned char)((bytes_por_seg >> 8) & 0xFF);
    buf[30] = (unsigned char)((bytes_por_seg >> 16) & 0xFF);
    buf[31] = (unsigned char)((bytes_por_seg >> 24) & 0xFF);
    buf[32] = (unsigned char)(alinhamento_bloco & 0xFF);
    buf[33] = (unsigned char)((alinhamento_bloco >> 8) & 0xFF);
    buf[34] = 16;
    buf[35] = 0;
    buf[36] = 'd';
    buf[37] = 'a';
    buf[38] = 't';
    buf[39] = 'a';
    buf[40] = (unsigned char)(tamanho_dados & 0xFF);
    buf[41] = (unsigned char)((tamanho_dados >> 8) & 0xFF);
    buf[42] = (unsigned char)((tamanho_dados >> 16) & 0xFF);
    buf[43] = (unsigned char)((tamanho_dados >> 24) & 0xFF);
}

static short *amostras_em_buffer(unsigned char *buf)
{
    return (short *)(buf + AUDIO_WAV_CABECALHO_BYTES);
}

static void gerar_tom_ambiente(unsigned char *buf, int num_amostras)
{
    int i;
    short *amostras = amostras_em_buffer(buf);
    escrever_cabecalho_wav(buf, num_amostras);

    for (i = 0; i < num_amostras; i++)
    {
        double t = (double)i / AUDIO_TAXA_AMOSTRA;
        double v = 0.0;

        v += sin(2.0 * M_PI * 55.0 * t) * 0.28;
        v += sin(2.0 * M_PI * 110.0 * t) * 0.10;
        v += sin(2.0 * M_PI * 165.0 * t) * 0.04;
        v += sin(2.0 * M_PI * 220.0 * t) * 0.02;

        if (i < AUDIO_TAXA_AMOSTRA / 20)
            v *= (double)i / (AUDIO_TAXA_AMOSTRA / 20);
        else if (i > num_amostras - AUDIO_TAXA_AMOSTRA / 20)
            v *= (double)(num_amostras - i) / (AUDIO_TAXA_AMOSTRA / 20);

        amostras[i] = (short)(v * 32000.0);
    }
}

static void gerar_tiro(unsigned char *buf, int num_amostras)
{
    int i;
    short *amostras = amostras_em_buffer(buf);
    escrever_cabecalho_wav(buf, num_amostras);

    for (i = 0; i < num_amostras; i++)
    {
        double t = (double)i / num_amostras;
        double freq = 900.0 - 500.0 * t;
        double v = sin(2.0 * M_PI * freq * (double)i / AUDIO_TAXA_AMOSTRA) * (1.0 - t) * 0.55;
        amostras[i] = (short)(v * 32000.0);
    }
}

static void gerar_impacto(unsigned char *buf, int num_amostras)
{
    int i;
    short *amostras = amostras_em_buffer(buf);
    escrever_cabecalho_wav(buf, num_amostras);

    for (i = 0; i < num_amostras; i++)
    {
        double t = (double)i / num_amostras;
        double envelope = pow(1.0 - t, 2.5);
        double v = sin(2.0 * M_PI * 140.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.65;
        v += sin(2.0 * M_PI * 280.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.15;
        amostras[i] = (short)(v * 32000.0);
    }
}

static void gerar_pulo(unsigned char *buf, int num_amostras)
{
    int i;
    short *amostras = amostras_em_buffer(buf);
    escrever_cabecalho_wav(buf, num_amostras);

    for (i = 0; i < num_amostras; i++)
    {
        double t = (double)i / num_amostras;
        double freq = 220.0 + 380.0 * t;
        double envelope = 1.0 - t * 0.5;
        double v = sin(2.0 * M_PI * freq * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.40;
        amostras[i] = (short)(v * 32000.0);
    }
}

static void gerar_aterrissagem(unsigned char *buf, int num_amostras)
{
    int i;
    short *amostras = amostras_em_buffer(buf);
    escrever_cabecalho_wav(buf, num_amostras);

    for (i = 0; i < num_amostras; i++)
    {
        double t = (double)i / num_amostras;
        double envelope = pow(1.0 - t, 3.5);
        double v = sin(2.0 * M_PI * 80.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.55;
        v += sin(2.0 * M_PI * 160.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.10;
        amostras[i] = (short)(v * 32000.0);
    }
}

void audio_inicializar(void)
{
#ifdef _WIN32
    gerar_tom_ambiente(s_buf_bgm, AUDIO_AMOSTRAS_BGM);
    gerar_tiro(s_buf_tiro, AUDIO_AMOSTRAS_TIRO);
    gerar_impacto(s_buf_impacto, AUDIO_AMOSTRAS_IMPACTO);
    gerar_pulo(s_buf_pulo, AUDIO_AMOSTRAS_PULO);
    gerar_aterrissagem(s_buf_aterrissagem, AUDIO_AMOSTRAS_ATERR);
#endif
}

void audio_definir_ativacao(int ativado)
{
    s_audioAtivado = ativado ? 1 : 0;

    if (!s_audioAtivado)
        audio_parar_musica();
}

void audio_tocar_musica(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buf_bgm, NULL,
               SND_MEMORY | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
#endif
}

void audio_parar_musica(void)
{
#ifdef _WIN32
    PlaySoundA(NULL, NULL, 0);
#endif
}

void audio_tocar_som_tiro_disparo(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buf_tiro, NULL,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
#endif
}

void audio_tocar_som_tiro_atingido(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buf_impacto, NULL,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
#endif
}

void audio_tocar_som_pulo_inicio(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buf_pulo, NULL,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
#endif
}

void audio_tocar_som_pulo_fim(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buf_aterrissagem, NULL,
               SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
#endif
}

int audio_esta_ativado(void)
{
    return s_audioAtivado;
}
