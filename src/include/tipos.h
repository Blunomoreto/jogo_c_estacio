#ifndef TIPOS_H
#define TIPOS_H

#include "configuracao.h"

typedef struct Vetor2D
{
    float x;
    float y;
} Vetor2D;

typedef struct Color
{
    float r;
    float g;
    float b;
    float a;
} Color;

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
    MELHORIA_TAXA_DE_DISPARO,
    MELHORIA_VELOCIDADE,
    MELHORIA_CURAR,
    MELHORIA_TEMPO,
    MELHORIA_ORIENTACAO_PONTO_PONTO,
    MELHORIA_ORIENTACAO_ANGULO_PONTO,
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

typedef enum TipoOrientacao
{
    ORIENTACAO_NENHUMA = 0,
    ORIENTACAO_PONTO_PONTO,
    ORIENTACAO_ANGULO_PONTO
} TipoOrientacao;

typedef struct EntradaPontuacao
{
    char nome[24];
    int pontuacao;
    int onda;
} EntradaPontuacao;

typedef struct EstadoEntrada
{
    unsigned char teclas[256];
    unsigned char teclasPressionadas[256];
    unsigned char especiais[256];
    unsigned char mouseApertado[3];
    unsigned char mousePressionado[3];
    int mouseX;
    int mouseY;
} EstadoEntrada;

typedef struct Player
{
    Vetor2D pos;
    float tamanho;
    float vida;
    float vidaMaxima;
    float velocidade;
    float dano;
    float taxaDeDisparo;
    float tempoRecargaDisparo;

    float velocidadeProjetil;
    float velocidadeY;
    int estaNoChao;
    float tempoTeclaPuloPressionada;
    Vetor2D vel;
    Vetor2D aceleracao;

    int possuiPontoPonto;
    int possuiAnguloPonto;
    int municaoGuiada;
    int municaoGuiadaMaxima;
    float aceleracaoLateralMaxima;
} Player;

typedef struct Inimigo
{
    int ativo;
    int ehChefao;
    TipoInimigo tipo;
    Vetor2D centro;
    float raioOrbita;
    float angulo;
    float velocidadeAngular;
    float tamanho;
    float vida;
    float vidaMaxima;
    float dano;
    float tempoRecargaDisparo;
    float flashDano;

    int contadorRajada;
    float temporizadorRajada;

    Vetor2D vel;
    Vetor2D aceleracao;
} Inimigo;

typedef struct Projetil
{
    int ativo;
    int vemDoJogador;
    Vetor2D pos;
    Vetor2D vel;
    float raio;
    float vida;
    float dano;

    TipoOrientacao orientacao;
    int indiceAlvo;
    float aceleracaoLateralAtual;
    float temporizadorCombustivel;
    float distanciaAnterior;
    int errou;
    float temporizadorDeteccao;
    float aceleracaoLateralMaxima;
} Projetil;

typedef struct Particula
{
    int ativo;
    Vetor2D pos;
    Vetor2D vel;
    float tamanho;
    float vida;
    Color cor;
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
    int ultimosTicks;
    float tempoDelta;

    Player jogador;
    Inimigo inimigos[MAXIMO_INIMIGOS];
    Projetil projetis[MAXIMO_PROJETEIS];
    Particula particulas[MAXIMO_PARTICULAS];
    Obstaculo obstaculos[MAXIMO_PLATAFORMAS];

    int onda;
    int inimigosRestantes;
    int ondasParaVencer;

    float tempoRestante;
    float tempoDecorrido;
    int pontuacao;
    int ouro;
    float flashDano;
    float flashMelhoria;
    int melhoriaSelecionada;
    char ultimaMelhoria[64];
    float temporizadorUltimaMelhoria;
    char mensagemToast[128];
    float temporizadorToast;
    int audioHabilitado;
    int dificuldade;

    OpcaoMelhoria melhorias[MAXIMO_OPCOES_UPGRADE];

    int maiorPontuacao;
    int maiorOndaAtingida;
    EntradaPontuacao topPontuacoes[5];
    int contadorTopPontuacoes;
    EntradaPontuacao todasPontuacoes[64];
    int contadorTodasPontuacoes;
    int paginaPontuacao;
    int tamanhoPaginaPontuacao;

    char nomeJogador[24];
    int inserindoNome;
    int nomeSalvo;

    unsigned int texturaFundo;
    int texturaFundoCarregada;
} Jogo;

#endif