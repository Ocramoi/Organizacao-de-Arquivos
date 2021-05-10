#include "Linhas.h"
#include "LeLinha.h"

#include <stdio.h>
#include <string.h>

/* Escreve [arquivo] binário a partir da [tabela] dada */
int escreveTabelaLinhas(FILE *arquivo, LINHAS_t *tabela) {
    return 0;
}

/* Adiciona linha a partir de [registro] à [tabela] já criada */
LINHA_t *adicionaLinha(LINHAS_t *tabela, char *registro) {
    // Estrutura para novo linha a ser adicionado
    LINHA_t *novo = malloc(sizeof(LINHA_t));

    // Retorna registro adicionado
    return novo;
}

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaLinhas(char *entrada, char *saida) {
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
    LINHAS_t *tabela = malloc(sizeof(LINHAS_t));
    tabela->status = 0;
    tabela->byteProxReg = 0;
    tabela->nroRegistros = 0;
    tabela->nroRegRemovidos = 0;
    strncpy(tabela->descreveCodigo, registro, 15); strtok(NULL, ",");
    strncpy(tabela->descreveCartao, registro, 13); strtok(NULL, ",");
    strncpy(tabela->descreveNome, registro, 13); strtok(NULL, ",");
    strncpy(tabela->descreveLinha, registro, 24);
    tabela->linhas = NULL;

    // Lê registros do csv linha a linha
    while (registro) {
        free(registro);
        registro = leLinha(arqEntrada);
        if (!registro) break;

        if (adicionaLinha(tabela, registro))
            printf("Erro na inserção de registro!\n");
    }
    // Fecha arquivo de entrada
    fclose(arqEntrada);

    tabela->status = 1; // TODO: Conferir se o status da tabela tá sendo indicado de acordo com especifiações ujiadfsguiabguib

    // Abre arquivo de saída
    FILE *arqSaida = fopen(saida, "wb+");
    // Escreve tabela
    escreveTabelaLinhas(arqSaida, tabela);
    // Fecha arquivo
    fclose(arqSaida);

    // Retorno de sucesso
    return 0;
}
