#include "audio.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

static int s_audioEnabled = 1;

void audio_init(void)
{
}

void audio_set_enabled(int enabled)
{
    s_audioEnabled = enabled ? 1 : 0;
    if (!s_audioEnabled)
    {
        audio_stop_bgm();
    }
}

int audio_is_enabled(void)
{
    return s_audioEnabled;
}

void audio_play_bgm(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/bgm.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
#endif
}

void audio_stop_bgm(void)
{
#ifdef _WIN32
    PlaySoundA(NULL, NULL, 0);
#endif
}

void audio_play_shoot(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/shoot.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_play_hit(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/hit.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_play_jump(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/jump.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}

void audio_play_land(void)
{
    if (!s_audioEnabled)
    {
        return;
    }
#ifdef _WIN32
    PlaySoundA("assets/audio/land.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
#endif
}
