#ifndef TIPOS_H
#define TIPOS_H

#include "configuracao.h"

typedef struct Vetor2D
{
    float x;
    float y;
} Vetor2D;

typedef struct Cor
{
    float r;
    float g;
    float b;
    float a;
} Cor;

typedef enum TelaJogo
{
    TELA_MENU = 0,
    TELA_OPCOES,
    TELA_PONTUACOES,
    TELA_JOGANDO,
    TELA_PAUSADA,
    TELA_MELHORIA,
    TELA_VITORIA,
    TELA_DERROTA
} TelaJogo;

typedef enum TipoMelhoria
{
    MELHORIA_DANO = 0,
    MELHORIA_TAXA_DISPARO,
    MELHORIA_VELOCIDADE,
    MELHORIA_CURA,
    MELHORIA_TEMPO,
    MELHORIA_GUIANCA_PP,
    MELHORIA_GUIANCA_APN,
    MELHORIA_MUNICAO,
    MELHORIA_CONTADOR
} TipoMelhoria;

typedef enum TipoInimigo
{
    INIMIGO_PADRAO = 0,
    INIMIGO_ATIRADOR,
    INIMIGO_TANQUE,
    INIMIGO_DIAMANTE,
    INIMIGO_PENTAGONO
} TipoInimigo;

typedef enum TipoGuianca
{
    GUIANCA_NENHUMA = 0,
    GUIANCA_PP,
    GUIANCA_APN
} TipoGuianca;

typedef struct RegistroPontuacao
{
    char nome[24];
    int pontuacao;
    int onda;
} RegistroPontuacao;

typedef struct EstadoEntrada
{
    unsigned char teclas[256];
    unsigned char teclas_pressionadas[256];
    unsigned char especiais[256];
    unsigned char mouse_segurado[3];
    unsigned char mouse_clicado[3];
    int mouse_x;
    int mouse_y;
} EstadoEntrada;

typedef struct Jogador
{
    Vetor2D pos;
    float tamanho;
    float vida;
    float vida_maxima;
    float velocidade;
    float dano;
    float taxa_disparo;
    float tempo_recarga_disparo;

    float velocidade_projetil;
    float velocidade_y;
    int esta_no_chao;
    float tempo_tecla_pulo_pressionada;
    Vetor2D vel;
    Vetor2D aceleracao;

    int tem_guianca_pp;
    int tem_guianca_apn;
    int municao_guiada;
    int municao_guiada_max;
    float aceleracao_lateral_max;
} Jogador;

typedef struct Inimigo
{
    int ativo;
    int eh_chefao;
    TipoInimigo tipo;
    Vetor2D centro;
    float raio_orbita;
    float angulo;
    float velocidade_angular;
    float tamanho;
    float vida;
    float vida_maxima;
    float dano;
    float tempo_recarga_disparo;
    float flash_dano;

    int contador_rajada;
    float tempo_entre_rajada;

    Vetor2D vel;
    Vetor2D aceleracao;
} Inimigo;

typedef struct Projetil
{
    int ativo;
    int vem_do_jogador;
    Vetor2D pos;
    Vetor2D vel;
    float raio;
    float vida;
    float dano;

    TipoGuianca guianca;
    int indice_alvo;
    float aceleracao_lateral;
    float tempo_com_combustivel;
    float distancia_anterior;
    int errou;
    float temporizador_deteccao;
    float aceleracao_lateral_max;
} Projetil;

typedef struct Particula
{
    int ativo;
    Vetor2D pos;
    Vetor2D vel;
    float tamanho;
    float vida;
    Cor cor;
} Particula;

typedef struct Obstaculo
{
    int ativo;
    float x;
    float y;
    float largura;
    float altura;
} Obstaculo;

typedef struct OpcaoMelhoria
{
    TipoMelhoria tipo;
    char rotulo[64];
    char descricao[96];
} OpcaoMelhoria;

typedef struct Jogo
{
    TelaJogo tela;
    EstadoEntrada entrada;

    int largura;
    int altura;

    int executando;
    int ultimos_ticks;
    float tempo_delta;

    Jogador jogador;
    Inimigo inimigos[MAXIMO_INIMIGOS];
    Projetil projetis[MAXIMO_PROJETEIS];
    Particula particulas[MAXIMO_PARTICULAS];
    Obstaculo obstaculos[MAXIMO_PLATAFORMAS];

    int onda;
    int inimigos_restantes;
    int ondas_para_vencer;

    float tempo_restante;
    float tempo_decorrido;
    int pontuacao;
    int ouro;
    float flash_dano;
    float flash_melhoria;
    int melhoria_selecionada;
    char ultima_melhoria[64];
    float temporizador_ultima_melhoria;
    char mensagem_toast[128];
    float temporizador_toast;
    int audio_habilitado;
    int dificuldade;

    OpcaoMelhoria melhorias[MAXIMO_OPCOES_UPGRADE];

    int recorde_pontuacao_maxima;
    int recorde_onda_maxima;
    RegistroPontuacao melhores_pontuacoes[5];
    int numero_melhores_pontuacoes;
    RegistroPontuacao todas_pontuacoes[64];
    int numero_todas_pontuacoes;
    int pagina_pontuacao;
    int itens_por_pagina_pontuacao;

    char nome_jogador[24];
    int inserindo_nome;
    int nome_salvo;

    unsigned int textura_fundo;
    int textura_fundo_carregado;
} Jogo;

#endif
