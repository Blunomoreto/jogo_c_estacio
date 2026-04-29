#include "audio.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

static int s_audioEnabled = 1;

void audio_inicializar(void)
{
}

void audio_definir_ativacao(int enabled)
{
    s_audioEnabled = enabled ? 1 : 0;
    if (!s_audioEnabled)
    {
        audio_parar_musica();
    }
}

int audio_esta_ativado(void)
{
    return s_audioEnabled;
}

void audio_tocar_musica(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/bgm.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
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
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/shoot.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_tocar_som_tiro_atingido(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/hit.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_tocar_som_pulo_inicio(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/jump.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_tocar_som_pulo_fim(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/land.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}
