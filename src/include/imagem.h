#ifndef IMAGEM_H
#define IMAGEM_H

unsigned int imagem_carregar_ppm(const char *caminho_arquivo, int *ok);
unsigned int imagem_salvar_ppm(const char *nome_arquivo, int largura, int altura);
unsigned int imagem_carregar_stb(const char *caminho, int *ok);
unsigned int imagem_carregar_fundo(int *ok);
unsigned int imagem_salvar_png(const char *caminho, int largura, int altura);

void imagem_gerar_fundo_estelar(const char *caminho, int largura, int altura);

#endif
