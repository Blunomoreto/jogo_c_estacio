#ifndef AUDIO_H
#define AUDIO_H

void audio_init(void);
void audio_set_enabled(int enabled);
int audio_is_enabled(void);
void audio_play_bgm(void);
void audio_stop_bgm(void);
void audio_play_shoot(void);
void audio_play_hit(void);
void audio_play_jump(void);
void audio_play_land(void);

#endif
