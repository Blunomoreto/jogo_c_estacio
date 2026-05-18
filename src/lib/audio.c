#include "audio.h"
#include "configuracao.h"

#include <math.h>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <string.h>
#pragma comment(lib, "winmm.lib")
#endif

static int s_audio_ativado = 1;
static int s_musica_tocando = 0;

static unsigned char s_buffer_musica[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_BGM)];
static unsigned char s_buffer_tiro[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_TIRO)];
static unsigned char s_buffer_impacto[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_IMPACTO)];
static unsigned char s_buffer_pulo[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_PULO)];
static unsigned char s_buffer_aterrissagem[AUDIO_TAMANHO_BUFFER(AUDIO_AMOSTRAS_ATERR)];

#ifdef _WIN32
static HWAVEOUT s_saida_onda_musica = NULL;
static WAVEHDR s_cabecalho_onda_musica;

static void CALLBACK audio_retorno_onda(HWAVEOUT saida_onda, UINT tipo_mensagem, DWORD_PTR nao_utilizado, DWORD_PTR parametro_cabecalho, DWORD_PTR nao_utilizado2)
{
    (void)nao_utilizado;
    (void)nao_utilizado2;
    if (tipo_mensagem == WOM_DONE && s_musica_tocando)
        waveOutWrite(saida_onda, (LPWAVEHDR)parametro_cabecalho, sizeof(WAVEHDR));
}
#endif

static void audio_escrever_cabecalho_wav(unsigned char *buffer, int numero_amostras)
{
    int tamanho_dados = numero_amostras * AUDIO_BYTES_POR_AMOSTRA;
    int tamanho_bloco = 36 + tamanho_dados;
    int bytes_por_segundo = AUDIO_TAXA_AMOSTRA * AUDIO_BYTES_POR_AMOSTRA;
    int alinhamento_bloco = AUDIO_BYTES_POR_AMOSTRA;

    buffer[0] = 'R';
    buffer[1] = 'I';
    buffer[2] = 'F';
    buffer[3] = 'F';
    buffer[4] = (unsigned char)(tamanho_bloco & 0xFF);
    buffer[5] = (unsigned char)((tamanho_bloco >> 8) & 0xFF);
    buffer[6] = (unsigned char)((tamanho_bloco >> 16) & 0xFF);
    buffer[7] = (unsigned char)((tamanho_bloco >> 24) & 0xFF);
    buffer[8] = 'W';
    buffer[9] = 'A';
    buffer[10] = 'V';
    buffer[11] = 'E';
    buffer[12] = 'f';
    buffer[13] = 'm';
    buffer[14] = 't';
    buffer[15] = ' ';
    buffer[16] = 16;
    buffer[17] = 0;
    buffer[18] = 0;
    buffer[19] = 0;
    buffer[20] = 1;
    buffer[21] = 0;
    buffer[22] = 1;
    buffer[23] = 0;
    buffer[24] = (unsigned char)(AUDIO_TAXA_AMOSTRA & 0xFF);
    buffer[25] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 8) & 0xFF);
    buffer[26] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 16) & 0xFF);
    buffer[27] = (unsigned char)((AUDIO_TAXA_AMOSTRA >> 24) & 0xFF);
    buffer[28] = (unsigned char)(bytes_por_segundo & 0xFF);
    buffer[29] = (unsigned char)((bytes_por_segundo >> 8) & 0xFF);
    buffer[30] = (unsigned char)((bytes_por_segundo >> 16) & 0xFF);
    buffer[31] = (unsigned char)((bytes_por_segundo >> 24) & 0xFF);
    buffer[32] = (unsigned char)(alinhamento_bloco & 0xFF);
    buffer[33] = (unsigned char)((alinhamento_bloco >> 8) & 0xFF);
    buffer[34] = 16;
    buffer[35] = 0;
    buffer[36] = 'd';
    buffer[37] = 'a';
    buffer[38] = 't';
    buffer[39] = 'a';
    buffer[40] = (unsigned char)(tamanho_dados & 0xFF);
    buffer[41] = (unsigned char)((tamanho_dados >> 8) & 0xFF);
    buffer[42] = (unsigned char)((tamanho_dados >> 16) & 0xFF);
    buffer[43] = (unsigned char)((tamanho_dados >> 24) & 0xFF);
}

static short *audio_amostras_em_buffer(unsigned char *buffer)
{
    return (short *)(buffer + AUDIO_WAV_CABECALHO_BYTES);
}

static void audio_gerar_marcha_militar(unsigned char *buffer, int numero_amostras)
{
    static const double frequencias_notas[8] = {
        196.00, 246.94, 293.66, 392.00,
        293.66, 246.94, 196.00, 146.83};

    const int amostras_por_batida = AUDIO_TAXA_AMOSTRA / 2;
    unsigned int estado_ruido = 0xACE1u;
    double fase_melodia = 0.0;
    int i;
    short *amostras = audio_amostras_em_buffer(buffer);
    audio_escrever_cabecalho_wav(buffer, numero_amostras);

    for (i = 0; i < numero_amostras; i++)
    {
        int indice_batida = (i / amostras_por_batida) % 8;
        int amostra_batida = i % amostras_por_batida;
        double tempo_batida = (double)amostra_batida / AUDIO_TAXA_AMOSTRA;
        double fase_batida = (double)amostra_batida / amostras_por_batida;
        double valor = 0.0;

        fase_melodia += 2.0 * M_PI * frequencias_notas[indice_batida] / AUDIO_TAXA_AMOSTRA;
        {
            double envelope_melodia = (fase_batida < 0.80) ? 1.0 : (1.0 - fase_batida) / 0.20;
            valor += sin(fase_melodia) * envelope_melodia * 0.16;
        }

        if ((indice_batida & 1) == 0)
        {
            if (tempo_batida < 0.14)
            {
                double acento = (indice_batida == 0 || indice_batida == 4) ? 1.0 : 0.70;
                double envelope_bumbo = pow(1.0 - tempo_batida / 0.14, 3.0);
                double frequencia_bumbo = 80.0 - 40.0 * (tempo_batida / 0.14);
                valor += sin(2.0 * M_PI * frequencia_bumbo * tempo_batida) * envelope_bumbo * 0.55 * acento;
                valor += sin(2.0 * M_PI * frequencia_bumbo * 1.5 * tempo_batida) * envelope_bumbo * 0.12 * acento;
            }
        }

        if ((indice_batida & 1) == 1)
        {
            if (tempo_batida < 0.09)
            {
                double envelope_caixa, ruido_branco, tom_corpo;
                envelope_caixa = pow(1.0 - tempo_batida / 0.09, 2.0);
                estado_ruido = estado_ruido * 1664525u + 1013904223u;
                ruido_branco = (double)((int)(estado_ruido >> 16) - 32768) / 32768.0;
                tom_corpo = sin(2.0 * M_PI * 220.0 * tempo_batida);
                valor += (ruido_branco * 0.40 + tom_corpo * 0.50) * envelope_caixa * 0.32;
            }
        }

        if (i < AUDIO_TAXA_AMOSTRA / 20)
            valor *= (double)i / (AUDIO_TAXA_AMOSTRA / 20);
        else if (i > numero_amostras - AUDIO_TAXA_AMOSTRA / 20)
            valor *= (double)(numero_amostras - i) / (AUDIO_TAXA_AMOSTRA / 20);

        amostras[i] = (short)(valor * 32000.0);
    }
}

static void audio_gerar_tiro(unsigned char *buffer, int numero_amostras)
{
    int i;
    short *amostras = audio_amostras_em_buffer(buffer);
    audio_escrever_cabecalho_wav(buffer, numero_amostras);

    for (i = 0; i < numero_amostras; i++)
    {
        double proporcao = (double)i / numero_amostras;
        double frequencia = 900.0 - 500.0 * proporcao;
        double valor = sin(2.0 * M_PI * frequencia * (double)i / AUDIO_TAXA_AMOSTRA) * (1.0 - proporcao) * 0.55;
        amostras[i] = (short)(valor * 32000.0);
    }
}

static void audio_gerar_impacto(unsigned char *buffer, int numero_amostras)
{
    int i;
    short *amostras = audio_amostras_em_buffer(buffer);
    audio_escrever_cabecalho_wav(buffer, numero_amostras);

    for (i = 0; i < numero_amostras; i++)
    {
        double proporcao = (double)i / numero_amostras;
        double envelope = pow(1.0 - proporcao, 3.5);
        double frequencia = 100.0 - 60.0 * proporcao;
        double valor = sin(2.0 * M_PI * frequencia * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.70;
        valor += sin(2.0 * M_PI * frequencia * 2.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.18;
        amostras[i] = (short)(valor * 32000.0);
    }
}

static void audio_gerar_pulo(unsigned char *buffer, int numero_amostras)
{
    int i;
    short *amostras = audio_amostras_em_buffer(buffer);
    audio_escrever_cabecalho_wav(buffer, numero_amostras);

    for (i = 0; i < numero_amostras; i++)
    {
        double proporcao = (double)i / numero_amostras;
        double frequencia = 220.0 + 380.0 * proporcao;
        double envelope = 1.0 - proporcao * 0.5;
        double valor = sin(2.0 * M_PI * frequencia * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.40;
        amostras[i] = (short)(valor * 32000.0);
    }
}

static void audio_gerar_aterrissagem(unsigned char *buffer, int numero_amostras)
{
    int i;
    short *amostras = audio_amostras_em_buffer(buffer);
    audio_escrever_cabecalho_wav(buffer, numero_amostras);

    for (i = 0; i < numero_amostras; i++)
    {
        double proporcao = (double)i / numero_amostras;
        double envelope = pow(1.0 - proporcao, 3.5);
        double valor = sin(2.0 * M_PI * 80.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.55;
        valor += sin(2.0 * M_PI * 160.0 * (double)i / AUDIO_TAXA_AMOSTRA) * envelope * 0.10;
        amostras[i] = (short)(valor * 32000.0);
    }
}

void audio_inicializar(void)
{
    audio_gerar_marcha_militar(s_buffer_musica, AUDIO_AMOSTRAS_BGM);
    audio_gerar_tiro(s_buffer_tiro, AUDIO_AMOSTRAS_TIRO);
    audio_gerar_impacto(s_buffer_impacto, AUDIO_AMOSTRAS_IMPACTO);
    audio_gerar_pulo(s_buffer_pulo, AUDIO_AMOSTRAS_PULO);
    audio_gerar_aterrissagem(s_buffer_aterrissagem, AUDIO_AMOSTRAS_ATERR);
}

void audio_definir_ativacao(int ativado)
{
    s_audio_ativado = ativado ? 1 : 0;
    if (!s_audio_ativado)
        audio_parar_musica();
}

void audio_atualizar(float delta_tempo)
{
    (void)delta_tempo;
}

void audio_tocar_musica(void)
{
#ifdef _WIN32
    WAVEFORMATEX formato_onda;
    if (!s_audio_ativado)
        return;
    audio_parar_musica();
    formato_onda.wFormatTag = WAVE_FORMAT_PCM;
    formato_onda.nChannels = 1;
    formato_onda.nSamplesPerSec = AUDIO_TAXA_AMOSTRA;
    formato_onda.wBitsPerSample = 16;
    formato_onda.nBlockAlign = AUDIO_BYTES_POR_AMOSTRA;
    formato_onda.nAvgBytesPerSec = AUDIO_TAXA_AMOSTRA * AUDIO_BYTES_POR_AMOSTRA;
    formato_onda.cbSize = 0;
    if (waveOutOpen(&s_saida_onda_musica, WAVE_MAPPER, &formato_onda,
                    (DWORD_PTR)audio_retorno_onda, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
        return;
    memset(&s_cabecalho_onda_musica, 0, sizeof(WAVEHDR));
    s_cabecalho_onda_musica.lpData = (LPSTR)(s_buffer_musica + AUDIO_WAV_CABECALHO_BYTES);
    s_cabecalho_onda_musica.dwBufferLength = AUDIO_AMOSTRAS_BGM * AUDIO_BYTES_POR_AMOSTRA;
    waveOutPrepareHeader(s_saida_onda_musica, &s_cabecalho_onda_musica, sizeof(WAVEHDR));
    waveOutWrite(s_saida_onda_musica, &s_cabecalho_onda_musica, sizeof(WAVEHDR));
    s_musica_tocando = 1;
#endif
}

void audio_parar_musica(void)
{
#ifdef _WIN32
    if (!s_saida_onda_musica)
        return;
    s_musica_tocando = 0;
    waveOutReset(s_saida_onda_musica);
    waveOutUnprepareHeader(s_saida_onda_musica, &s_cabecalho_onda_musica, sizeof(WAVEHDR));
    waveOutClose(s_saida_onda_musica);
    s_saida_onda_musica = NULL;
#endif
}

void audio_tocar_som_tiro_disparo(void)
{
    if (!s_audio_ativado)
        return;
#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buffer_tiro, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
#endif
}

void audio_tocar_som_tiro_atingido(void)
{
    if (!s_audio_ativado)
        return;
#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buffer_impacto, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
#endif
}

void audio_tocar_som_pulo_inicio(void)
{
    if (!s_audio_ativado)
        return;
#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buffer_pulo, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
#endif
}

void audio_tocar_som_pulo_fim(void)
{
    if (!s_audio_ativado)
        return;
#ifdef _WIN32
    PlaySoundA((LPCSTR)s_buffer_aterrissagem, NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
#endif
}

int audio_esta_ativado(void)
{
    return s_audio_ativado;
}
