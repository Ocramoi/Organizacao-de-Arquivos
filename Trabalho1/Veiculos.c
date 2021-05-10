#include "Veiculos.h"
#include "LeLinha.h"

#include <stdio.h>
#include <string.h>

/* Escreve [arquivo] binário a partir da [tabela] dada */
int escreveTabelaVeiculos(FILE *arquivo, VEICULOS_t *tabela) {
    return 0;
}

/* Adiciona veículo a partir de [registro] à [tabela] já criada */
VEICULO_t *adicionaVeiculo(VEICULOS_t *tabela, char *registro) {
    // Estrutura para novo veículo a ser adicionado
    VEICULO_t *novo = malloc(sizeof(VEICULO_t));

    // Retorna registro adicionado
    return novo;
}

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaVeiculos(char *entrada, char *saida) {
    // Abre arquivo de entrada a partir do nome
    FILE *arqEntrada = fopen(entrada, "rb");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        // Fecha arquivos e retorna erro;
        fclose(arqEntrada);
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *registro = leLinha(arqEntrada);
    // Inicia tokenização do csv
    strtok(registro, ",");

    // Cria nova estrutura para a tabela a ser lida e popula dados
    VEICULOS_t *tabela = malloc(sizeof(VEICULOS_t));
    tabela->status = 0;
    tabela->byteProxReg = 0;
    tabela->nroRegistros = 0;
    tabela->nroRegRemovidos = 0;
    strncpy(tabela->descrevePrefixo, registro, 18); strtok(NULL, ",");
    strncpy(tabela->descreveData, registro, 35); strtok(NULL, ",");
    strncpy(tabela->descreveLugares, registro, 42); strtok(NULL, ",");
    strncpy(tabela->descreveLinha, registro, 17); strtok(NULL, ",");
    strncpy(tabela->descreveModelo, registro, 20); strtok(NULL, ",");
    strncpy(tabela->descreveCategoria, registro, 26);
    tabela->veiculos = NULL;

    // Lê registros do csv linha a linha
    while (registro) {
        free(registro);
        registro = leLinha(arqEntrada);
        if (!registro) break;

        if (adicionaVeiculo(tabela, registro))
            printf("Erro na inserção de registro!\n");
    }
    // Fecha arquivo de entrada
    fclose(arqEntrada);

    tabela->status = 1; // TODO: Conferir se o status da tabela tá sendo indicado de acordo com especifiações ujiadfsguiabguib

    // Abre arquivo de saída
    FILE *arqSaida = fopen(saida, "wb+");
    // Escreve tabela
    escreveTabelaVeiculos(arqSaida, tabela);
    // Fecha arquivo
    fclose(arqSaida);

    // Retorno de sucesso
    return 0;
}
