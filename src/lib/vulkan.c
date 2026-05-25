#include "vulkan.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

static VkInstance instancia_vulkan = VK_NULL_HANDLE;
static VkPhysicalDevice dispositivo_fisico_vulkan = VK_NULL_HANDLE;
static VkDevice dispositivo_logico_vulkan = VK_NULL_HANDLE;
static VkQueue fila_grafica_vulkan = VK_NULL_HANDLE;
static uint32_t indice_familia_fila_grafica = 0;
static int vulkan_foi_inicializado = 0;

static void vulkan_desenhar_padrao_alternativo(unsigned char *pixels, int largura, int altura)
{
    int linha;
    int coluna;
    for (linha = 0; linha < altura; ++linha)
    {
        for (coluna = 0; coluna < largura; ++coluna)
        {
            int indice = (linha * largura + coluna) * 4;
            float fator_horizontal = (float)coluna / (float)(largura - 1);
            float fator_vertical = (float)linha / (float)(altura - 1);
            float distancia_centro_quadrada = (fator_horizontal - 0.5f) * (fator_horizontal - 0.5f) + (fator_vertical - 0.5f) * (fator_vertical - 0.5f);
            unsigned char componente_vermelho = (unsigned char)(40.0f + 180.0f * fator_horizontal);
            unsigned char componente_verde = (unsigned char)(20.0f + 200.0f * (1.0f - fator_vertical));
            unsigned char componente_azul = (unsigned char)(160.0f + 95.0f * (1.0f - distancia_centro_quadrada * 2.0f));
            unsigned char componente_alpha = 220;
            if (linha < 2 || linha >= altura - 2 || coluna < 2 || coluna >= largura - 2)
            {
                componente_vermelho = 255;
                componente_verde = 255;
                componente_azul = 255;
                componente_alpha = 255;
            }
            pixels[indice + 0] = componente_vermelho;
            pixels[indice + 1] = componente_verde;
            pixels[indice + 2] = componente_azul;
            pixels[indice + 3] = componente_alpha;
        }
    }
}

static void vulkan_preencher_padrao_quadriculado(unsigned char *pixels, int largura, int altura)
{
    int linha;
    int coluna;
    int tamanho_bloco_quadriculado = 8;
    for (linha = 0; linha < altura; ++linha)
    {
        for (coluna = 0; coluna < largura; ++coluna)
        {
            int indice = (linha * largura + coluna) * 4;
            int bloco_linha = linha / tamanho_bloco_quadriculado;
            int bloco_coluna = coluna / tamanho_bloco_quadriculado;
            int quadriculado_par = (bloco_linha + bloco_coluna) & 1;
            if (linha < 2 || linha >= altura - 2 || coluna < 2 || coluna >= largura - 2)
            {
                pixels[indice + 0] = 255;
                pixels[indice + 1] = 255;
                pixels[indice + 2] = 255;
                pixels[indice + 3] = 255;
            }
            else if (quadriculado_par)
            {
                pixels[indice + 0] = 30;
                pixels[indice + 1] = 110;
                pixels[indice + 2] = 220;
                pixels[indice + 3] = 230;
            }
            else
            {
                pixels[indice + 0] = 15;
                pixels[indice + 1] = 60;
                pixels[indice + 2] = 140;
                pixels[indice + 3] = 230;
            }
        }
    }
}

static int vulkan_buscar_tipo_memoria_compativel(uint32_t filtro_tipos_aceitos, VkMemoryPropertyFlags propriedades_desejadas, uint32_t *indice_tipo_encontrado)
{
    VkPhysicalDeviceMemoryProperties propriedades_memoria_dispositivo;
    uint32_t indice_tipo;
    vkGetPhysicalDeviceMemoryProperties(dispositivo_fisico_vulkan, &propriedades_memoria_dispositivo);
    for (indice_tipo = 0; indice_tipo < propriedades_memoria_dispositivo.memoryTypeCount; ++indice_tipo)
    {
        if ((filtro_tipos_aceitos & (1u << indice_tipo)) &&
            (propriedades_memoria_dispositivo.memoryTypes[indice_tipo].propertyFlags & propriedades_desejadas) == propriedades_desejadas)
        {
            *indice_tipo_encontrado = indice_tipo;
            return 1;
        }
    }
    return 0;
}

int vulkan_esta_disponivel(void)
{
    return vulkan_foi_inicializado;
}

int vulkan_inicializar(void)
{
    VkApplicationInfo informacoes_aplicacao;
    VkInstanceCreateInfo informacoes_instancia;
    uint32_t numero_dispositivos_disponiveis = 0;
    VkPhysicalDevice dispositivos_disponiveis[8];
    uint32_t indice_dispositivo;
    uint32_t indice_familia;
    uint32_t numero_familias_de_filas = 0;
    VkQueueFamilyProperties familias_de_filas[16];
    float prioridade_fila = 1.0f;
    VkDeviceQueueCreateInfo informacoes_fila;
    VkDeviceCreateInfo informacoes_dispositivo;
    VkResult resultado;

    if (vulkan_foi_inicializado)
        return 1;

    memset(&informacoes_aplicacao, 0, sizeof(informacoes_aplicacao));
    informacoes_aplicacao.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    informacoes_aplicacao.pApplicationName = "Orbit Siege Vulkan Overlay";
    informacoes_aplicacao.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    informacoes_aplicacao.pEngineName = "OrbitSiegeEngine";
    informacoes_aplicacao.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    informacoes_aplicacao.apiVersion = VK_API_VERSION_1_0;

    memset(&informacoes_instancia, 0, sizeof(informacoes_instancia));
    informacoes_instancia.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    informacoes_instancia.pApplicationInfo = &informacoes_aplicacao;

    resultado = vkCreateInstance(&informacoes_instancia, NULL, &instancia_vulkan);
    if (resultado != VK_SUCCESS)
    {
        fprintf(stderr, "[vulkan] falha em vkCreateInstance (codigo %d)\n", (int)resultado);
        instancia_vulkan = VK_NULL_HANDLE;
        return 0;
    }

    resultado = vkEnumeratePhysicalDevices(instancia_vulkan, &numero_dispositivos_disponiveis, NULL);
    if (resultado != VK_SUCCESS || numero_dispositivos_disponiveis == 0)
    {
        fprintf(stderr, "[vulkan] nenhum dispositivo encontrado\n");
        vkDestroyInstance(instancia_vulkan, NULL);
        instancia_vulkan = VK_NULL_HANDLE;
        return 0;
    }
    if (numero_dispositivos_disponiveis > 8)
        numero_dispositivos_disponiveis = 8;
    vkEnumeratePhysicalDevices(instancia_vulkan, &numero_dispositivos_disponiveis, dispositivos_disponiveis);
    dispositivo_fisico_vulkan = dispositivos_disponiveis[0];
    for (indice_dispositivo = 0; indice_dispositivo < numero_dispositivos_disponiveis; ++indice_dispositivo)
    {
        VkPhysicalDeviceProperties propriedades_candidato;
        vkGetPhysicalDeviceProperties(dispositivos_disponiveis[indice_dispositivo], &propriedades_candidato);
        if (propriedades_candidato.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            dispositivo_fisico_vulkan = dispositivos_disponiveis[indice_dispositivo];
            break;
        }
    }

    {
        VkPhysicalDeviceProperties propriedades_dispositivo_escolhido;
        vkGetPhysicalDeviceProperties(dispositivo_fisico_vulkan, &propriedades_dispositivo_escolhido);
        printf("[vulkan] usando GPU: %s\n", propriedades_dispositivo_escolhido.deviceName);
    }

    vkGetPhysicalDeviceQueueFamilyProperties(dispositivo_fisico_vulkan, &numero_familias_de_filas, NULL);
    if (numero_familias_de_filas > 16)
        numero_familias_de_filas = 16;
    vkGetPhysicalDeviceQueueFamilyProperties(dispositivo_fisico_vulkan, &numero_familias_de_filas, familias_de_filas);
    indice_familia_fila_grafica = numero_familias_de_filas;
    for (indice_familia = 0; indice_familia < numero_familias_de_filas; ++indice_familia)
    {
        if (familias_de_filas[indice_familia].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indice_familia_fila_grafica = indice_familia;
            break;
        }
    }
    if (indice_familia_fila_grafica == numero_familias_de_filas)
    {
        fprintf(stderr, "[vulkan] nenhuma queue family grafica encontrada\n");
        vkDestroyInstance(instancia_vulkan, NULL);
        instancia_vulkan = VK_NULL_HANDLE;
        return 0;
    }

    memset(&informacoes_fila, 0, sizeof(informacoes_fila));
    informacoes_fila.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    informacoes_fila.queueFamilyIndex = indice_familia_fila_grafica;
    informacoes_fila.queueCount = 1;
    informacoes_fila.pQueuePriorities = &prioridade_fila;

    memset(&informacoes_dispositivo, 0, sizeof(informacoes_dispositivo));
    informacoes_dispositivo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    informacoes_dispositivo.queueCreateInfoCount = 1;
    informacoes_dispositivo.pQueueCreateInfos = &informacoes_fila;

    resultado = vkCreateDevice(dispositivo_fisico_vulkan, &informacoes_dispositivo, NULL, &dispositivo_logico_vulkan);
    if (resultado != VK_SUCCESS)
    {
        fprintf(stderr, "[vulkan] falha em vkCreateDevice (codigo %d)\n", (int)resultado);
        vkDestroyInstance(instancia_vulkan, NULL);
        instancia_vulkan = VK_NULL_HANDLE;
        return 0;
    }

    vkGetDeviceQueue(dispositivo_logico_vulkan, indice_familia_fila_grafica, 0, &fila_grafica_vulkan);
    vulkan_foi_inicializado = 1;
    return 1;
}

int vulkan_gerar_sobreposicao_rgba(unsigned char *buffer_pixels, int largura, int altura)
{
    VkBuffer buffer_intermediario = VK_NULL_HANDLE;
    VkDeviceMemory memoria_intermediaria = VK_NULL_HANDLE;
    VkImage imagem_fora_de_tela = VK_NULL_HANDLE;
    VkDeviceMemory memoria_imagem = VK_NULL_HANDLE;
    VkCommandPool pool_de_comandos = VK_NULL_HANDLE;
    VkCommandBuffer buffer_de_comandos = VK_NULL_HANDLE;
    VkBufferCreateInfo informacoes_buffer;
    VkImageCreateInfo informacoes_imagem;
    VkMemoryRequirements requisitos_memoria;
    VkMemoryAllocateInfo informacoes_alocacao;
    uint32_t indice_tipo_memoria;
    VkCommandPoolCreateInfo informacoes_pool;
    VkCommandBufferAllocateInfo informacoes_buffer_comando;
    VkCommandBufferBeginInfo informacoes_inicio;
    VkBufferImageCopy descricao_copia_buffer_imagem;
    VkImageMemoryBarrier barreira_para_destino;
    VkImageMemoryBarrier barreira_para_origem;
    VkSubmitInfo informacoes_envio;
    VkClearColorValue cor_de_fundo;
    VkImageSubresourceRange subrecurso_completo;
    void *ponteiro_mapeado = NULL;
    size_t bytes_sobreposicao;
    int sucesso = 0;

    if (!buffer_pixels || largura <= 0 || altura <= 0)
        return 0;

    if (!vulkan_foi_inicializado)
    {
        vulkan_desenhar_padrao_alternativo(buffer_pixels, largura, altura);
        return 1;
    }

    bytes_sobreposicao = (size_t)largura * (size_t)altura * 4;

    vulkan_preencher_padrao_quadriculado(buffer_pixels, largura, altura);

    memset(&informacoes_buffer, 0, sizeof(informacoes_buffer));
    informacoes_buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    informacoes_buffer.size = bytes_sobreposicao;
    informacoes_buffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    informacoes_buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(dispositivo_logico_vulkan, &informacoes_buffer, NULL, &buffer_intermediario) != VK_SUCCESS)
        goto liberar_recursos;

    vkGetBufferMemoryRequirements(dispositivo_logico_vulkan, buffer_intermediario, &requisitos_memoria);
    if (!vulkan_buscar_tipo_memoria_compativel(requisitos_memoria.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                               &indice_tipo_memoria))
        goto liberar_recursos;
    memset(&informacoes_alocacao, 0, sizeof(informacoes_alocacao));
    informacoes_alocacao.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    informacoes_alocacao.allocationSize = requisitos_memoria.size;
    informacoes_alocacao.memoryTypeIndex = indice_tipo_memoria;
    if (vkAllocateMemory(dispositivo_logico_vulkan, &informacoes_alocacao, NULL, &memoria_intermediaria) != VK_SUCCESS)
        goto liberar_recursos;
    vkBindBufferMemory(dispositivo_logico_vulkan, buffer_intermediario, memoria_intermediaria, 0);

    if (vkMapMemory(dispositivo_logico_vulkan, memoria_intermediaria, 0, bytes_sobreposicao, 0, &ponteiro_mapeado) != VK_SUCCESS)
        goto liberar_recursos;
    memcpy(ponteiro_mapeado, buffer_pixels, bytes_sobreposicao);
    vkUnmapMemory(dispositivo_logico_vulkan, memoria_intermediaria);
    ponteiro_mapeado = NULL;

    memset(&informacoes_imagem, 0, sizeof(informacoes_imagem));
    informacoes_imagem.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    informacoes_imagem.imageType = VK_IMAGE_TYPE_2D;
    informacoes_imagem.format = VK_FORMAT_R8G8B8A8_UNORM;
    informacoes_imagem.extent.width = (uint32_t)largura;
    informacoes_imagem.extent.height = (uint32_t)altura;
    informacoes_imagem.extent.depth = 1;
    informacoes_imagem.mipLevels = 1;
    informacoes_imagem.arrayLayers = 1;
    informacoes_imagem.samples = VK_SAMPLE_COUNT_1_BIT;
    informacoes_imagem.tiling = VK_IMAGE_TILING_OPTIMAL;
    informacoes_imagem.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    informacoes_imagem.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    informacoes_imagem.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (vkCreateImage(dispositivo_logico_vulkan, &informacoes_imagem, NULL, &imagem_fora_de_tela) != VK_SUCCESS)
        goto liberar_recursos;

    vkGetImageMemoryRequirements(dispositivo_logico_vulkan, imagem_fora_de_tela, &requisitos_memoria);
    if (!vulkan_buscar_tipo_memoria_compativel(requisitos_memoria.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                               &indice_tipo_memoria))
        goto liberar_recursos;
    memset(&informacoes_alocacao, 0, sizeof(informacoes_alocacao));
    informacoes_alocacao.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    informacoes_alocacao.allocationSize = requisitos_memoria.size;
    informacoes_alocacao.memoryTypeIndex = indice_tipo_memoria;
    if (vkAllocateMemory(dispositivo_logico_vulkan, &informacoes_alocacao, NULL, &memoria_imagem) != VK_SUCCESS)
        goto liberar_recursos;
    vkBindImageMemory(dispositivo_logico_vulkan, imagem_fora_de_tela, memoria_imagem, 0);

    memset(&informacoes_pool, 0, sizeof(informacoes_pool));
    informacoes_pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    informacoes_pool.queueFamilyIndex = indice_familia_fila_grafica;
    informacoes_pool.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (vkCreateCommandPool(dispositivo_logico_vulkan, &informacoes_pool, NULL, &pool_de_comandos) != VK_SUCCESS)
        goto liberar_recursos;

    memset(&informacoes_buffer_comando, 0, sizeof(informacoes_buffer_comando));
    informacoes_buffer_comando.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    informacoes_buffer_comando.commandPool = pool_de_comandos;
    informacoes_buffer_comando.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    informacoes_buffer_comando.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(dispositivo_logico_vulkan, &informacoes_buffer_comando, &buffer_de_comandos) != VK_SUCCESS)
        goto liberar_recursos;

    memset(&informacoes_inicio, 0, sizeof(informacoes_inicio));
    informacoes_inicio.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    informacoes_inicio.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(buffer_de_comandos, &informacoes_inicio);

    memset(&barreira_para_destino, 0, sizeof(barreira_para_destino));
    barreira_para_destino.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barreira_para_destino.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barreira_para_destino.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barreira_para_destino.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barreira_para_destino.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barreira_para_destino.image = imagem_fora_de_tela;
    barreira_para_destino.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barreira_para_destino.subresourceRange.levelCount = 1;
    barreira_para_destino.subresourceRange.layerCount = 1;
    barreira_para_destino.srcAccessMask = 0;
    barreira_para_destino.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    vkCmdPipelineBarrier(buffer_de_comandos, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, NULL, 0, NULL, 1, &barreira_para_destino);

    cor_de_fundo.float32[0] = 0.05f;
    cor_de_fundo.float32[1] = 0.10f;
    cor_de_fundo.float32[2] = 0.20f;
    cor_de_fundo.float32[3] = 1.0f;
    memset(&subrecurso_completo, 0, sizeof(subrecurso_completo));
    subrecurso_completo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subrecurso_completo.levelCount = 1;
    subrecurso_completo.layerCount = 1;
    vkCmdClearColorImage(buffer_de_comandos, imagem_fora_de_tela, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         &cor_de_fundo, 1, &subrecurso_completo);

    memset(&descricao_copia_buffer_imagem, 0, sizeof(descricao_copia_buffer_imagem));
    descricao_copia_buffer_imagem.bufferOffset = 0;
    descricao_copia_buffer_imagem.bufferRowLength = 0;
    descricao_copia_buffer_imagem.bufferImageHeight = 0;
    descricao_copia_buffer_imagem.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    descricao_copia_buffer_imagem.imageSubresource.layerCount = 1;
    descricao_copia_buffer_imagem.imageExtent.width = (uint32_t)largura;
    descricao_copia_buffer_imagem.imageExtent.height = (uint32_t)altura;
    descricao_copia_buffer_imagem.imageExtent.depth = 1;
    vkCmdCopyBufferToImage(buffer_de_comandos, buffer_intermediario, imagem_fora_de_tela,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &descricao_copia_buffer_imagem);

    memset(&barreira_para_origem, 0, sizeof(barreira_para_origem));
    barreira_para_origem.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barreira_para_origem.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barreira_para_origem.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barreira_para_origem.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barreira_para_origem.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barreira_para_origem.image = imagem_fora_de_tela;
    barreira_para_origem.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barreira_para_origem.subresourceRange.levelCount = 1;
    barreira_para_origem.subresourceRange.layerCount = 1;
    barreira_para_origem.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barreira_para_origem.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(buffer_de_comandos, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, NULL, 0, NULL, 1, &barreira_para_origem);

    vkCmdCopyImageToBuffer(buffer_de_comandos, imagem_fora_de_tela,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer_intermediario, 1, &descricao_copia_buffer_imagem);

    vkEndCommandBuffer(buffer_de_comandos);

    memset(&informacoes_envio, 0, sizeof(informacoes_envio));
    informacoes_envio.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    informacoes_envio.commandBufferCount = 1;
    informacoes_envio.pCommandBuffers = &buffer_de_comandos;
    if (vkQueueSubmit(fila_grafica_vulkan, 1, &informacoes_envio, VK_NULL_HANDLE) != VK_SUCCESS)
        goto liberar_recursos;
    vkQueueWaitIdle(fila_grafica_vulkan);

    if (vkMapMemory(dispositivo_logico_vulkan, memoria_intermediaria, 0, bytes_sobreposicao, 0, &ponteiro_mapeado) != VK_SUCCESS)
        goto liberar_recursos;
    memcpy(buffer_pixels, ponteiro_mapeado, bytes_sobreposicao);
    vkUnmapMemory(dispositivo_logico_vulkan, memoria_intermediaria);
    ponteiro_mapeado = NULL;
    sucesso = 1;

liberar_recursos:
    if (buffer_de_comandos != VK_NULL_HANDLE && pool_de_comandos != VK_NULL_HANDLE)
        vkFreeCommandBuffers(dispositivo_logico_vulkan, pool_de_comandos, 1, &buffer_de_comandos);
    if (pool_de_comandos != VK_NULL_HANDLE)
        vkDestroyCommandPool(dispositivo_logico_vulkan, pool_de_comandos, NULL);
    if (imagem_fora_de_tela != VK_NULL_HANDLE)
        vkDestroyImage(dispositivo_logico_vulkan, imagem_fora_de_tela, NULL);
    if (memoria_imagem != VK_NULL_HANDLE)
        vkFreeMemory(dispositivo_logico_vulkan, memoria_imagem, NULL);
    if (buffer_intermediario != VK_NULL_HANDLE)
        vkDestroyBuffer(dispositivo_logico_vulkan, buffer_intermediario, NULL);
    if (memoria_intermediaria != VK_NULL_HANDLE)
        vkFreeMemory(dispositivo_logico_vulkan, memoria_intermediaria, NULL);

    if (!sucesso)
    {
        fprintf(stderr, "[vulkan] falha durante geracao - usando padrao alternativo\n");
        vulkan_desenhar_padrao_alternativo(buffer_pixels, largura, altura);
    }
    return 1;
}

void vulkan_finalizar(void)
{
    if (dispositivo_logico_vulkan != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(dispositivo_logico_vulkan);
        vkDestroyDevice(dispositivo_logico_vulkan, NULL);
        dispositivo_logico_vulkan = VK_NULL_HANDLE;
    }
    if (instancia_vulkan != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instancia_vulkan, NULL);
        instancia_vulkan = VK_NULL_HANDLE;
    }
    vulkan_foi_inicializado = 0;
}
