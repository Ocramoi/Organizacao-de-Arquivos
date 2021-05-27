/*
** SCC0215 - Organização de arquivos
** Trabalho 1
**
** Alunos:
** - Marco Antônio Ribeiro de Toledo, RA: 11796419
** - Clayton Miccas Junior, RA: 9292441
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinarioNaTela.h"

// Bibliotecas implementadas
#include "LeLinha.h"
#include "Veiculos.h"
#include "Linhas.h"

int main(int argc, char **argv) {
    // Lê opção, chamando então função desejada
    int opcao;
    char *entrada = leLinha(stdin);
    sscanf(entrada, "%d", &opcao);
    switch (opcao) {
        // Funções [1] e [2] -> "CREATE TABLE"
        case 1: {
            // Lê entrada
            char arqEntrada[100],
                 arqSaida[100];
            sscanf(entrada, "%d %s %s", &opcao, arqEntrada, arqSaida);
            // Trata retorno da função
            if (!criaTabelaVeiculosARQ(arqEntrada, arqSaida))
                FF_binarioNaTela(arqSaida);
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        case 2: {
            // Lê entrada
            char arqEntrada[100],
                 arqSaida[100];
            sscanf(entrada, "%d %s %s", &opcao, arqEntrada, arqSaida);
            // Trata retorno da função
            if (!criaTabelaLinhas(arqEntrada, arqSaida))
                FF_binarioNaTela(arqSaida);
            else
                printf("Falha no processamento do arquivo.\n");
            break;
        }
        // Funções [3] e [4] -> "Select * FROM"
        case 3: {
            // Lê entrada
            char arqEntrada[100];
            sscanf(entrada, "%d %s", &opcao, arqEntrada);
            // Processa possíveis saídas de erro
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
            // Lê entrada
            char arqEntrada[100];
            sscanf(entrada, "%d %s", &opcao, arqEntrada);
            // Processa possíveis saídas de erro
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
        // Funções [5] e [6] -> "SELECT * from WHERE"
        case 5: {
            // Lê entrada
            char arqEntrada[100],
                 nomeDoCampo[100],
                 valor[100];
            sscanf(entrada, "%d %s %s %s", &opcao, arqEntrada, nomeDoCampo, valor);
            // Processa possíveis saídas de erro
            switch (selectVeiculos(arqEntrada, nomeDoCampo, valor)) {
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
        case 6: {
            // Lê entrada
            char arqEntrada[100],
                 nomeDoCampo[100],
                 valor[100];
            sscanf(entrada, "%d %s %s %s", &opcao, arqEntrada, nomeDoCampo, valor);
            // Processa possíveis saídas de erro
            switch (selectLinhas(arqEntrada, nomeDoCampo, valor)) {
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
        // Funções [7] e [8] -> "INSERT INTO"
        case 7: {
            // Lê entrada
            char arqTabela[100];
            int num,
                ret;
            sscanf(entrada, "%d %s %d", &opcao, arqTabela, &num);
            // Insere registros linha a linha
            for (int i = 0; i < num; ++i) {
                char *registro = leLinha(stdin);
                ret = insertVeiculo(arqTabela, registro);
                free(registro);
                if (ret)
                    break;
            }
            // Trata retorno
            if (ret)
                printf("Falha no processamento do arquivo.\n");
            else
                FF_binarioNaTela(arqTabela);

            break;
        }
        case 8: {
            // Lê entrada
            char arqTabela[100];
            int num,
                ret;
            sscanf(entrada, "%d %s %d", &opcao, arqTabela, &num);
            // Insere registros linha a linha
            for (int i = 0; i < num; ++i) {
                char *registro = leLinha(stdin);
                ret = insertLinha(arqTabela, registro);
                free(registro);
                if (ret)
                    break;
            }
            // Trata retorno
            if (ret)
                printf("Falha no processamento do arquivo.\n");
            else
                FF_binarioNaTela(arqTabela);

            break;
        }
        default:
            printf("Opção digitada inválida!\n");
            break;
    }

    // Libera memória dinamicamente alocada
    free(entrada);
    return 0;
}
