#ifndef IMAGEM_H
#define IMAGEM_H

unsigned int imagem_carregar_ppm(const char *caminho_arquivo, int *ok);
unsigned int imagem_salvar_ppm(const char *nome_arquivo, int largura, int altura);

#endif
