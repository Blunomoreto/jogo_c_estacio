#ifndef VULKAN_H
#define VULKAN_H

int vulkan_esta_disponivel(void);
int vulkan_inicializar(void);
int vulkan_gerar_sobreposicao_rgba(unsigned char *buffer_pixels, int largura, int altura);
void vulkan_finalizar(void);

#endif
