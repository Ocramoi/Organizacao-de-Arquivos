/*
** NOTE: Funções implementadas: [1]
** TODO: Implementar estrutura das linhas individuais em Linhas.h
** REVIEW: Conferir criaTabelaVeiculos em Veiculos.c (estrutura passível de mudança pós implementação das outras funções)
** REVIEW: Conferir sintaxe e estrutura dos TADs
** REVIEW: Testar funções de tabela de veículo
** 
** Comentarios Clayton: Adicionei a função binarioNaTela, estou implementando a [2] Linhas.c, estou usando o seu Veiculos.c como base assim cada um trabalha em uma parte. 
** Hoje a noite vou dar uma boa codada kkk 
**
** Comentarios Marco: Função [1] implementada mas não testada, cuido disso depois da aula ijafsdngijuadbnfg
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
            if (criaTabelaVeiculos(arqEntrada, arqSaida))
                FF_binarioNaTela(arqSaida);
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        case 2:
            break;
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
