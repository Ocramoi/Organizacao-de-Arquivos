/*
** NOTE: Funções implementadas
** TODO: Implementar escreveTabelaVeiculos em Veiculos.c 
** TODO: Implementar adicionaVeiculo em Veiculos.c
** TODO: Implementar escrita das linhas no binário
** REVIEW: Conferir criaTabelaVeiculos em Veiculos.c (estrutura passível de mudança pós implementação das outras funções)
** REVIEW: Conferir sintaxe e estrutura dos TADs
** 
** Comentarios Clayton: Adicionei a função binarioNaTela, estou implementando a [2] Linhas.c, começarei a implementar a escrita no binário
** 
**
** Comentarios Marco:
**
 */

/*
** SCC0215 - Organização de arquivos
** Trabalho 1
**
** Alunos:
** - Marco Antônio Ribeiro de Toledo, RA: 11796419
** - Clayton Miccas Junior, RA: 9292441
 */

#include "LeLinha.h"
#include "Veiculos.h"
#include "Linhas.h"
#include "BinarioNaTela.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    // Lê opção e dados necessários, chamando então função desejada
    int opcao;
    char *entrada = leLinha(stdin);
    sscanf(entrada, "%d", &opcao);
    switch (opcao) {
        // Funções [1] e [2] -> "CREATE TABLE"
        case 1: {
            char arqEntrada[100],
                 arqSaida[100];
            sscanf(entrada, "%d %s %s", &opcao, arqEntrada, arqSaida);
            if (criaTabelaVeiculos(arqEntrada, arqSaida)) {
                printf("Função ok!\n"); 
                FF_binarioNaTela(arqSaida); // Função binarioNaTela desenvolvida pela Professora
            }
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        case 2: {
            char arqEntrada[100],
                 arqSaida[100];
            sscanf(entrada, "%d %s %s", &opcao, arqEntrada, arqSaida);
            if (criaTabelaLinhas(arqEntrada, arqSaida)) {
                printf("Função ok!\n"); 
                FF_binarioNaTela(arqSaida); // Função binarioNaTela desenvolvida pela Professora
            }
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            printf("Opção digitada inválida!\n");
            break;
    }

    // Libera memória dinamicamente alocada
    free(entrada);
    return 0;
}
