#ifndef AUDIO_H
#define AUDIO_H

void audio_inicializar(void);
void audio_definir_ativacao(int enabled);
void audio_tocar_musica(void);
void audio_parar_musica(void);
void audio_tocar_som_tiro_disparo(void);
void audio_tocar_som_tiro_atingido(void);
void audio_tocar_som_pulo_inicio(void);
void audio_tocar_som_pulo_fim(void);

int audio_esta_ativado(void);

#endif
