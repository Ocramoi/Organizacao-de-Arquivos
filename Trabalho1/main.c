/*
** NOTE: Funções implementadas: [1] e [2]
** TODO: Iniciar a implementação da função 3 sobre os dados de veiculos.
** TODO: Iniciar a implementação da função 4 sobre os dados de linha.
**  
** Comentarios Clayton: Os maiores problemas na hash eram sobre a gravação do tamanho do registro e byteofset, tbm na conferência da abertura do arquivo; 
** Importante: Uma curiosidade que não consegui encontrar o motivo, o byteofset da função [1] conta do valor 1 em diante, da função [2] conta do 0, talvez seja bom rever.
**
** Comentarios Marco: Função [1] implementada AGORA DIRETAMENTE DO ARQUIVO !! ligeiramente incorreto no hashing de conferência (vou arrumar amanhã)
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
#include "VeiculosARQ.h"
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
            if (!criaTabelaVeiculosARQ(arqEntrada, arqSaida))
                FF_binarioNaTela(arqSaida);
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        case 2: {
            char arqEntrada[100],
                 arqSaida[100];
            sscanf(entrada, "%d %s %s", &opcao, arqEntrada, arqSaida);
            if (!criaTabelaLinhas(arqEntrada, arqSaida))
                FF_binarioNaTela(arqSaida);
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