#include "Veiculos.h"
#include "LeLinha.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona veículo a partir de [tempRegistro] ao arquivo [tabela] já criado */
int adicionaVeiculoARQ(FILE *tabela, char *registro, int64_t *offset) {
    // Erro de ponteiros
    if (!tabela || !registro || !offset)
        return -1;

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro

    // Contabiliza e escreve se registro foi removido
    int controleRemovido;
    if (tempLeitura[0] == '*'){
        controleRemovido = 1;
        memmove(tempLeitura, tempLeitura+1, strlen(tempLeitura));
    }
    else
        controleRemovido = 0;
    fwrite(controleRemovido ? "0" : "1", sizeof(char), 1, tabela);

    // Captura e armazena os dados do arquivo CSV, fez-se o armazenamento para contabilizar o tamanho do arquivo
    char *prefixo = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *data = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *quantidadeLugares  = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *codLinha  = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *modelo = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *categoria = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    
    // Define-se o tamanho do campo Modelo
    int32_t controleModelo;
    if (!strcmp(modelo, "NULO"))
        controleModelo = 0;
    else
        controleModelo = strlen(modelo);
    
    // Define-se o tamanho do campo Categoria
    int32_t controleCat;
    if (!strcmp(categoria, "NULO"))
        controleCat = 0;
    else
        controleCat = strlen(categoria);
    
    // Definição e escrita do tamanho de registro 4*int+15*char = 31 + variável
    int32_t tamanhoRegistro;
    tamanhoRegistro = 31 + controleModelo + controleCat;
    fwrite(&tamanhoRegistro, sizeof(int32_t), 1, tabela);
    
    // Escrita do campo Prefixo.
    fwrite(prefixo, sizeof(char), 5, tabela); 

    // Escreve strings tratando NULO quando necessário e registrando informações numéricas
    if (!strcmp(data, "NULO"))
        fwrite("\0@@@@@@@@@", sizeof(char), 10, tabela);
    else
        fwrite(data, sizeof(char), 10, tabela);

    // Verificação da quantidade de lugares
    int32_t controleLugares;
    if (!strcmp(quantidadeLugares, "NULO"))
        controleLugares = -1;
    else
        controleLugares = atoi(quantidadeLugares);
    fwrite(&controleLugares, sizeof(int32_t), 1, tabela);

    // Verificação do codLinha
    int32_t controleLinha;
    if (!strcmp(codLinha, "NULO"))
        controleLinha = -1;
    else
        controleLinha = atoi(codLinha);
    
    // Escrita dos demais campos.
    fwrite(&controleLinha, sizeof(int32_t), 1, tabela);
    fwrite(&controleModelo, sizeof(int32_t), 1, tabela);
    fwrite(modelo, sizeof(char), controleModelo, tabela);
    fwrite(&controleCat, sizeof(int32_t), 1, tabela);
    fwrite(categoria, sizeof(char), controleCat, tabela);
    tempLeitura = strtok(NULL, ",");

    // Contabiliza tamanho do registro
    *offset += 36 + controleModelo + controleCat;

    // Retorna registro adicionado
    return controleRemovido;
}

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida) {
    // Abre arquivos a partir do nome
    FILE *arqEntrada = fopen(entrada, "r"),
        *arqSaida = fopen(saida, "wb+");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *tempRegistro = leLinha(arqEntrada),
    // Inicia tokenização do csv
    *registro = strtok(tempRegistro, ",");

    // Inicializa cabeçalho
    int64_t proxInit = 174;
    int32_t nRegInit = 0;
    fwrite("0", sizeof(char), 1, arqSaida);
    fwrite(&proxInit, sizeof(int64_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(registro, sizeof(char), 18, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 35, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 42, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 26, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 17, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 20, arqSaida);

    // Escreve registros do csv linha a linha
    int32_t nReg = 0, nRemovidos = 0;
    int64_t offset = 175;
    while (tempRegistro) {
        free(tempRegistro);
        tempRegistro = leLinha(arqEntrada);
        if (!tempRegistro) break;

        int r = adicionaVeiculoARQ(arqSaida, tempRegistro, &offset);
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
    nReg = nReg - nRemovidos;
    fwrite(&nReg, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRemovidos, sizeof(int32_t), 1, arqSaida);

    // Fecha arquivo de saída após escrever
    fclose(arqSaida);

    return 0;
}
