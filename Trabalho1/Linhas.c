#include "Linhas.h"
#include "LeLinha.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona linha a partir de [tempRegistro] ao arquivo [tabela] já criado */
int adicionaLinha(FILE *tabela, char *registro, int64_t *offset) {
    // Erro de ponteiros
    if (!tabela || !registro || !offset)
        return -1;

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro
    
    int32_t codigo;
    // Contabiliza e escreve se registro foi removido e escreve o código nas duas situações
    int controleRemovido;
    if (tempLeitura[0] == '*'){
        controleRemovido = 1;
        fwrite("1", sizeof(char), 1, tabela);
        codigo = 2;
    }
    else{
        controleRemovido = 0;
        fwrite("0", sizeof(char), 1, tabela);
        codigo = atoi(tempLeitura);
    }
        fwrite(&codigo, sizeof(int32_t), 1, tabela);
        tempLeitura = strtok(NULL, ",");

    
    // Escreve strings tratando NULO quando necessário e registrando informações numéricas
    
    //if (!strcmp(tempLeitura, "NULO"))
    //    return -1; // ESSE CASO NÃO PODE ACONTECER, TRATAR COMO ??
    //else


    if (!strcmp(tempLeitura, "NULO"))
        fwrite("\0", sizeof(char), 1, tabela);
    else
        fwrite(tempLeitura, sizeof(char), 1, tabela);
    tempLeitura = strtok(NULL, ",");

    int32_t controleNomeLinha;
    if (!strcmp(tempLeitura, "NULO"))
        controleNomeLinha = 0;
    else
        controleNomeLinha = strlen(tempLeitura);
    fwrite(&controleNomeLinha, sizeof(int32_t), 1, tabela);
    fwrite(tempLeitura, sizeof(char), controleNomeLinha, tabela);
    tempLeitura = strtok(NULL, ",");

    int32_t controleCorLinha;
    if (!strcmp(tempLeitura, "NULO"))
        controleCorLinha = 0;
    else
        controleCorLinha = strlen(tempLeitura);
    fwrite(&controleCorLinha, sizeof(int32_t), 1, tabela);
    fwrite(tempLeitura, sizeof(char), controleCorLinha, tabela);
    tempLeitura = strtok(NULL, ",");

    // Contabiliza tamanho do registro
    *offset += 18 + controleNomeLinha + controleCorLinha;

    // Retorna registro adicionado
    return controleRemovido;
}

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaLinhas(char *entrada, char *saida) {
    // Abre arquivos a partir do nome
    FILE *arqEntrada = fopen(entrada, "r"),
         *arqSaida = fopen(saida, "wb+");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        // Fecha arquivos e retorna erro;
        fclose(arqEntrada); fclose(arqSaida);
        printf("::: 81\n");
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *tempRegistro = leLinha(arqEntrada),
    // Inicia tokenização do csv
    *registro = strtok(tempRegistro, ",");

    // Inicializa cabeçalho
    int64_t byteProxReg = 83;
    int32_t nroRegistros = 0;
    int32_t nroRegRemovidos = 0;
    fwrite("0", sizeof(char), 1, arqSaida);
    fwrite(&byteProxReg, sizeof(int64_t), 1, arqSaida);
    fwrite(&nroRegistros, sizeof(int32_t), 1, arqSaida);
    fwrite(&nroRegRemovidos, sizeof(int32_t), 1, arqSaida);
    fwrite(registro, sizeof(char), 15, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 13, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 13, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 24, arqSaida); 

    // Escreve registros do csv linha a linha
    int32_t nReg = 0, nRemovidos = 0;
    int64_t offset = 0;
    while (tempRegistro) {
        free(tempRegistro);
        tempRegistro = leLinha(arqEntrada);
        if (!tempRegistro) break;

        int r = adicionaLinha(arqSaida, tempRegistro, &offset);
        if (r == -1) {
            printf("::: 114\n");
            free(tempRegistro);
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
