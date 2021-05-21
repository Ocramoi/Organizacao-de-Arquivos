/*
** NOTE: Funções implementadas: [1], [2], [3], [4]
** TODO: Iniciar a implementação da função 5 e 6 sobre os dados.
**
** Comentarios Clayton: Os maiores problemas na hash eram sobre a gravação do tamanho do registro e byteofset, tbm na conferência da abertura do arquivo; 
** Importante: Uma curiosidade que não consegui encontrar o motivo, o byteofset da função [1] conta do valor 1 em diante, da função [2] conta do 0, talvez seja bom rever.
** Função [5] esboçada inicialmente, funciona com prefixo XXXXX, os outros casos não estão implementados...
**
** Comentários Marco: função 3 e 4 implementadas e funcionando, mas ainda não testadas no run.codes
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
        case 3: {
            char arqEntrada[100];
            sscanf(entrada, "%d %s", &opcao, arqEntrada);
            switch (selectAllVeiculos(arqEntrada)) {
                case -1:
                    printf("Registro inexistente.\n");
                    break;
                case 1:
                    printf("Falha no processamento do arquivo.\n");
                    break;
                default:
                    break;
            }
            break;
        }
        case 4: {
            char arqEntrada[100];
            sscanf(entrada, "%d %s", &opcao, arqEntrada);
            switch (selectAllLinhas(arqEntrada)) {
                case -1:
                    printf("Registro inexistente.\n");
                    break;
                case 1:
                    printf("Falha no processamento do arquivo.\n");
                    break;
                default:
                    break;
            }
            break;
        }
        case 5: {
            char arqEntrada[100],
                 nomeDoCampo[100],
                 valor[100];
            sscanf(entrada, "%d %s %s %s", &opcao, arqEntrada, nomeDoCampo, valor);
            /* if (!buscaVeiculos(arqEntrada, nomeDoCampo, valor)){ */
                
            /* } */
            /* else */
            /*     printf("Falha no processamento do arquivo.\n"); */
            break;
        }
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
