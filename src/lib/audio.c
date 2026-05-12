#include "audio.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

static int s_audioAtivado = 1;

void inicializar_audio(void)
{
}

void definir_audio_ativado(int ativado)
{
    s_audioAtivado = ativado ? 1 : 0;

    if (!s_audioAtivado)
        parar_musica();
}

int audio_esta_ativado(void)
{
    return s_audioAtivado;
}

void tocar_musica(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA("assets/audio/bgm.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
#endif
}

void parar_musica(void)
{
#ifdef _WIN32
    PlaySoundA(NULL, NULL, 0);
#endif
}

void tocar_som_tiro(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA("assets/audio/shoot.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void tocar_som_impacto(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA("assets/audio/hit.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void tocar_som_pulo_inicio(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA("assets/audio/jump.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void tocar_som_pulo_fim(void)
{
    if (!s_audioAtivado)
        return;

#ifdef _WIN32
    PlaySoundA("assets/audio/land.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}