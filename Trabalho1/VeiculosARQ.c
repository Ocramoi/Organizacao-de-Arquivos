#include "VeiculosARQ.h"
#include "LeLinha.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona veículo a partir de [registro] ao arquivo [tabela] já criado */
int adicionaVeiculoARQ(FILE *tabela, char *registro, int64_t *offset) {
    // Erro de ponteiros
    if (!tabela || !registro || !offset)
        return -1;

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro

    // Contabiliza e escreve se registro foi removido
    int controleRemovido;
    if (tempLeitura[0] == '*')
        controleRemovido = 1;
    else
        controleRemovido = 0;
    fwrite(controleRemovido ? "0" : "1", sizeof(char), 1, tabela);
    fwrite(tempLeitura, sizeof(char), 5, tabela); strtok(NULL, ",");

    // Escreve strings tratando NULO quando necessário e registrando informações numéricas
    if (!strcmp(tempLeitura, "NULO"))
        fwrite("\0@@@@@@@@@", sizeof(char), 10, tabela);
    else
        fwrite(tempLeitura, sizeof(char), 10, tabela);
    strtok(NULL, ",");

    int32_t controleLugares;
    if (!strcmp(tempLeitura, "NULO"))
        controleLugares = -1;
    else
        controleLugares = atoi(tempLeitura);
    strtok(NULL, ",");
    fwrite(&controleLugares, sizeof(int32_t), 1, tabela);

    int32_t controleLinha;
    if (!strcmp(tempLeitura, "NULO"))
        controleLinha = -1;
    else
        controleLinha = atoi(tempLeitura);
    strtok(NULL, ",");
    fwrite(&controleLinha, sizeof(int32_t), 1, tabela);

    int32_t controleModelo;
    if (!strcmp(tempLeitura, "NULO"))
        controleModelo = 0;
    else
        controleModelo = strlen(tempLeitura);
    strtok(NULL, ",");
    fwrite(tempLeitura, sizeof(char), controleModelo, tabela);

    int32_t controleCat;
    if (!strcmp(tempLeitura, "NULO"))
        controleCat = 0;
    else
        controleCat = strlen(tempLeitura);
    strtok(NULL, ",");
    fwrite(tempLeitura, sizeof(char), controleCat, tabela);

    // Contabiliza tamanho do registro
    *offset += 36 + controleModelo + controleCat;

    // Retorna registro adicionado
    return controleRemovido;
}

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida) {
    // Abre arquivos a partir do nome
    FILE *arqEntrada = fopen(entrada, "rb"),
        *arqSaida = fopen(saida, "wb+");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        // Fecha arquivos e retorna erro;
        fclose(arqEntrada); fclose(arqSaida);
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *registro = leLinha(arqEntrada);
    // Inicia tokenização do csv
    strtok(registro, ",");

    // Inicializa cabeçalho
    int64_t proxInit = 175;
    int32_t nRegInit = 0;
    fwrite("0", sizeof(char), 1, arqSaida);
    fwrite(&proxInit, sizeof(int64_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(registro, sizeof(char), 18, arqSaida); strtok(NULL, ",");
    fwrite(registro, sizeof(char), 35, arqSaida); strtok(NULL, ",");
    fwrite(registro, sizeof(char), 42, arqSaida); strtok(NULL, ",");
    fwrite(registro, sizeof(char), 26, arqSaida); strtok(NULL, ",");
    fwrite(registro, sizeof(char), 17, arqSaida); strtok(NULL, ",");
    fwrite(registro, sizeof(char), 20, arqSaida);

    // Escreve registros do csv linha a linha
    int32_t nReg = 0, nRemovidos = 0;
    int64_t offset = 0;
    while (registro) {
        free(registro);
        registro = leLinha(arqEntrada);
        if (!registro) break;

        int r = adicionaVeiculoARQ(arqSaida, registro, &offset);
        if (r == -1) {
            fclose(arqEntrada); fclose(arqSaida);
            return 1;
        }
        nRemovidos += r;
        nReg++;
    }
    // Fecha arquivo de entrada
    fclose(arqEntrada);

    // Atualiza cabeçalho
    fseek(arqSaida, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arqSaida);
    fwrite(&offset, sizeof(int64_t), 1, arqSaida);
    fwrite(&nReg, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRemovidos, sizeof(int32_t), 1, arqSaida);

    // Fecha arquivo de saída após escrever
    fclose(arqSaida);

    return 0;
}
