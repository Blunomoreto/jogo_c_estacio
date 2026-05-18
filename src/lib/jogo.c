#include "jogo.h"
#include "audio.h"
#include "cenario.h"
#include "configuracao.h"
#include "desenhar.h"
#include "imagem.h"
#include "inimigo.h"
#include "interface.h"
#include "matematica.h"
#include "persistencia.h"
#include "gameplay.h"
#include "renderizar.h"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void game_reset_player(Jogo *jogo)
{
    memset(&jogo->jogador, 0, sizeof(jogo->jogador));

    jogo->jogador.pos = matematica_vetor2d((float)jogo->largura * 0.5f, ALTURA_CHAO - 16.0f);
    jogo->jogador.tamanho = 16.0f;
    jogo->jogador.vida = JOGADOR_INICIAL_VIDA;
    jogo->jogador.vida_maxima = JOGADOR_INICIAL_VIDA;
    jogo->jogador.velocidade = JOGADOR_INICIAL_VELOCIDADE;
    jogo->jogador.dano = JOGADOR_INICIAL_DANO;
    jogo->jogador.taxa_disparo = JOGADOR_INICIAL_TAXA_DISPARO;
    jogo->jogador.tempo_recarga_disparo = 0.0f;
    jogo->jogador.velocidade_projetil = JOGADOR_INICIAL_VELOCIDADE_PROJETIL;
    jogo->jogador.velocidade_y = 0.0f;
    jogo->jogador.esta_no_chao = 1;
    jogo->jogador.tempo_tecla_pulo_pressionada = 0.0f;

    jogo->jogador.tem_guianca_pp = 0;
    jogo->jogador.tem_guianca_apn = 0;
    jogo->jogador.municao_guiada = 0;
    jogo->jogador.municao_guiada_max = 0;
    jogo->jogador.aceleracao_lateral_max = JOGADOR_INICIAL_ACELERACAO_LATERAL;
}

void jogo_iniciar(Jogo *jogo, int largura, int altura)
{
    memset(jogo, 0, sizeof(*jogo));
    srand((unsigned int)time(NULL));

    jogo->largura = largura;
    jogo->altura = altura;
    jogo->tela = TELA_MENU;
    jogo->executando = 1;
    jogo->audio_habilitado = 1;
    jogo->dificuldade = 1;
    jogo->pagina_pontuacao = 0;
    jogo->itens_por_pagina_pontuacao = 10;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    persistencia_carregar_stats(&jogo->recorde_pontuacao_maxima, &jogo->recorde_onda_maxima);
    persistencia_carregar_configuracoes(&jogo->audio_habilitado, &jogo->dificuldade);
    interface_refrescar_pontuacoes_maximas(jogo);
    interface_refrescar_pontuacoes(jogo);

    jogo->textura_fundo = imagem_carregar_fundo(&jogo->textura_fundo_carregado);
    audio_inicializar();
    audio_definir_ativacao(jogo->audio_habilitado);
}

void jogo_atualizar(Jogo *jogo, float delta_tempo)
{
    if (!jogo->executando)
        return;

    jogo->tempo_delta = delta_tempo;

    jogo->flash_melhoria -= delta_tempo * 1.7f;
    if (jogo->flash_melhoria < 0.0f)
        jogo->flash_melhoria = 0.0f;
    jogo->temporizador_ultima_melhoria -= delta_tempo;
    if (jogo->temporizador_ultima_melhoria < 0.0f)
        jogo->temporizador_ultima_melhoria = 0.0f;
    jogo->temporizador_toast -= delta_tempo;
    if (jogo->temporizador_toast < 0.0f)
        jogo->temporizador_toast = 0.0f;

    if (jogo->tela == TELA_JOGANDO)
        gameplay_atualizar_playing(jogo, delta_tempo);

    audio_atualizar(delta_tempo);
}

void jogo_renderizar(Jogo *jogo)
{
    int indice_inimigo;

    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderizar_perspectiva_ortografica(jogo);
    renderizar_fundo(jogo);

    if (jogo->tela == TELA_MENU)
        desenhar_menu(jogo);

    if (jogo->tela == TELA_PONTUACOES)
        desenhar_placar(jogo);

    if (jogo->tela == TELA_OPCOES)
        desenhar_opcoes(jogo);

    if (jogo->tela == TELA_JOGANDO || jogo->tela == TELA_PAUSADA ||
        jogo->tela == TELA_MELHORIA || jogo->tela == TELA_VITORIA || jogo->tela == TELA_DERROTA)
    {
        renderizar_plataformas(jogo);
        for (indice_inimigo = 0; indice_inimigo < MAXIMO_INIMIGOS; ++indice_inimigo)
        {
            if (jogo->inimigos[indice_inimigo].ativo)
                renderizar_inimigo(&jogo->inimigos[indice_inimigo]);
        }
        renderizar_projeteis(jogo);
        renderizar_particulas(jogo);
        renderizar_jogador(jogo);
        desenhar_hud(jogo);
        desenhar_vida_boss(jogo);
        if (jogo->tela == TELA_JOGANDO)
            desenhar_mira(jogo);
    }

    if (jogo->tela == TELA_PAUSADA)
        desenhar_pausa(jogo);

    if (jogo->tela == TELA_MELHORIA)
        desenhar_melhorias_tela(jogo);

    if (jogo->tela == TELA_VITORIA || jogo->tela == TELA_DERROTA)
        desenhar_fim(jogo);

    desenhar_flash(jogo);

    glutSwapBuffers();
}

void jogo_reiniciar(Jogo *jogo)
{
    game_reset_player(jogo);
    cenario_limpar_entidades(jogo);

    jogo->onda = 1;
    jogo->ondas_para_vencer = inimigo_ondas_vitoria(jogo->dificuldade);
    jogo->inimigos_restantes = 0;
    jogo->tempo_restante = inimigo_tempo_inicio(jogo->dificuldade);
    jogo->tempo_decorrido = 0.0f;
    jogo->pontuacao = 0;
    jogo->ouro = 0;
    jogo->flash_dano = 0.0f;
    jogo->flash_melhoria = 0.0f;
    jogo->melhoria_selecionada = -1;
    jogo->temporizador_ultima_melhoria = 0.0f;
    memset(jogo->ultima_melhoria, 0, sizeof(jogo->ultima_melhoria));
    jogo->temporizador_toast = 0.0f;
    memset(jogo->mensagem_toast, 0, sizeof(jogo->mensagem_toast));
    jogo->tela = TELA_JOGANDO;
    jogo->inserindo_nome = 0;
    jogo->nome_salvo = 0;

    memset(jogo->nome_jogador, 0, sizeof(jogo->nome_jogador));
    snprintf(jogo->nome_jogador, sizeof(jogo->nome_jogador), "Player");

    cenario_criar_onda(jogo);
    audio_tocar_musica();
}
