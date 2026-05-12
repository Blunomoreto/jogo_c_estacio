#include "interface.h"
#include "persistencia.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>

void interface_refrescar_pontuacoes_maximas(Jogo *jogo)
{
    jogo->numero_melhores_pontuacoes = persistencia_carregar_pontuacoes_altas(jogo->melhores_pontuacoes, 5);
}

void interface_refrescar_pontuacoes(Jogo *jogo)
{
    jogo->numero_todas_pontuacoes = persistencia_carregar_pontuacoes(jogo->todas_pontuacoes, 64);
    if (jogo->pagina_pontuacao < 0)
        jogo->pagina_pontuacao = 0;
}

void interface_notificar(Jogo *jogo, const char *mensagem)
{
    snprintf(jogo->mensagem_toast, sizeof(jogo->mensagem_toast), "%s", mensagem ? mensagem : "");
    jogo->temporizador_toast = 2.2f;
}

void interface_encerrar_partida(Jogo *jogo, TelaJogo fim)
{
    audio_parar_musica();
    jogo->tela = fim;
    jogo->inserindo_nome = 1;
    jogo->nome_salvo = 0;

    if (jogo->pontuacao > jogo->recorde_pontuacao_maxima)
        jogo->recorde_pontuacao_maxima = jogo->pontuacao;
    if (jogo->onda > jogo->recorde_onda_maxima)
        jogo->recorde_onda_maxima = jogo->onda;
    persistencia_salvar_stats(jogo->recorde_pontuacao_maxima, jogo->recorde_onda_maxima);
    interface_refrescar_pontuacoes_maximas(jogo);
}
