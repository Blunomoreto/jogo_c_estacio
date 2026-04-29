#ifndef IMAGEM_H
#define IMAGEM_H

unsigned int imagem_carregar_ppm(const char *path, int *ok);
unsigned int imagem_salvar_ppm(const char *filename, int width, int height);

#endif
