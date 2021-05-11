#include "Veiculos.h"
#include "LeLinha.h"

#include <stdio.h>
#include <string.h>

/* Libera toda a memória de elemento único */
int destroiVeiculo(VEICULO_t *veiculo) {
    // Confere se registro válido foi passado como argumento
    if (!veiculo)
        return 1;

    // Libera strings do registro
    free(veiculo->modelo); free(veiculo->categoria);
    // Libera memória fixa do registro
    free(veiculo);

    return 0;
}

/* Libera tabela, seus elementos e elementos desses */
int destroiTabelaVeiculos(VEICULOS_t *tabela) {
    // Confere se tabela válida foi passada como argumento
    if (!tabela)
        return 1;

    // Inicializa valor de retorno para sucesso
    int retorno = 0;

    // Libera cada registro individualmente e o ponteiro da lista
    for (int i = 0; i < tabela->nroRegistros; ++i) {
        // Registra erro em qualquer uma das desalocações
        if (destroiVeiculo(tabela->veiculos[i]))
            retorno = 1;
    } free(tabela->veiculos);

    // Libera memória fixa
    free(tabela);

    return retorno;
}

/* Escreve arquivo binário de nome [arquivo] a partir da [tabela] dada */
int escreveTabelaVeiculos(char *arquivo, VEICULOS_t *tabela) {
    // Abre arquivo de saída
    FILE *arqSaida = fopen(arquivo, "wb+");
    if (!arqSaida)
        return 1;

    // Escreve campos do cabeçalho
    fwrite(&tabela->status, sizeof(char), 1, arqSaida);
    fwrite(&tabela->byteProxReg, sizeof(int64_t), 1, arqSaida);
    fwrite(&tabela->nroRegistros, sizeof(int32_t), 1, arqSaida);
    fwrite(&tabela->nroRegRemovidos, sizeof(int32_t), 1, arqSaida);
    fwrite(tabela->descrevePrefixo, sizeof(char), 18, arqSaida);
    fwrite(tabela->descreveData, sizeof(char), 35, arqSaida);
    fwrite(tabela->descreveLugares, sizeof(char), 42, arqSaida);
    fwrite(tabela->descreveLinha, sizeof(char), 17, arqSaida);
    fwrite(tabela->descreveModelo, sizeof(char), 20, arqSaida);
    fwrite(tabela->descreveCategoria, sizeof(char), 26, arqSaida);

    // Copia lista de veículos para facilitar manipulação
    VEICULO_t **veiculos = tabela->veiculos;

    // Escreve elementos individuais da lista de veículos
    for (int i = 0; i < tabela->nroRegistros; ++i) {
        // Escreve campos do registro
        fwrite(&veiculos[i]->removido, sizeof(char), 1, arqSaida);
        fwrite(&veiculos[i]->tamanhoRegistro, sizeof(int32_t), 1, arqSaida);
        fwrite(veiculos[i]->prefixo, sizeof(char), 5, arqSaida);
        fwrite(veiculos[i]->data, sizeof(char), 10, arqSaida);
        fwrite(&veiculos[i]->quantidadeLugares, sizeof(int32_t), 1, arqSaida);
        fwrite(&veiculos[i]->codLinha, sizeof(int32_t), 1, arqSaida);
        fwrite(&veiculos[i]->tamanhoModelo, sizeof(int32_t), 1, arqSaida);
        fwrite(veiculos[i]->modelo, sizeof(char), veiculos[i]->tamanhoModelo, arqSaida);
        fwrite(&veiculos[i]->tamanhoCategoria, sizeof(int32_t), 1, arqSaida);
        fwrite(veiculos[i]->categoria, sizeof(char), veiculos[i]->tamanhoCategoria, arqSaida);
    }

    // Fecha arquivo
    fclose(arqSaida);
    return 0;
}

/* Adiciona veículo a partir de [registro] à [tabela] já criada */
VEICULO_t *adicionaVeiculo(VEICULOS_t *tabela, char *registro) {
    // Estrutura para novo veículo a ser adicionado
    VEICULO_t *novo = malloc(sizeof(VEICULO_t));

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro

    // Registra se registro foi removido
    if (tempLeitura[0] == '*')
        novo->removido = 0;

    // Copia prefixo de tamanho fixo (não nulo por padrão)
    memcpy(novo->prefixo, tempLeitura, 5); strtok(NULL, ",");

    // Copia data de tamanho fixo contabilizando por campo nulo
    if (!strcmp(tempLeitura, "NULO"))
        memcpy(novo->data, "\0@@@@", 5);
    else
        memcpy(novo->data, tempLeitura, 10);
    strtok(NULL, ",");

    // Copia número de lugares, contabilizando por campo nulo
    if (!strcmp(tempLeitura, "NULO"))
        novo->quantidadeLugares = -1;
    else
        novo->quantidadeLugares = atoi(tempLeitura);
    strtok(NULL, ",");

    // Copia código de linha, contabilizando por campo nulo
    if (!strcmp(tempLeitura, "NULO"))
        novo->codLinha = -1;
    else
        novo->codLinha = atoi(tempLeitura);
    strtok(NULL, ",");

    // Trata e copia string de modelo e seu tamanho
    if (!strcmp(tempLeitura, "NULO")) {
        novo->tamanhoModelo = 0;
        novo->modelo = NULL;
    }
    else {
        novo->tamanhoModelo = strlen(tempLeitura);
        novo->modelo = malloc(novo->tamanhoModelo);
        memcpy(novo->modelo, tempLeitura, novo->tamanhoModelo);
    }
    strtok(NULL, ",");

    // Trata e copia string de categoria e seu tamanho
    if (!strcmp(tempLeitura, "NULO")) {
        novo->tamanhoCategoria = 0;
        novo->categoria = NULL;
    }
    else {
        novo->tamanhoCategoria = strlen(tempLeitura);
        novo->categoria = malloc(novo->tamanhoCategoria);
        memcpy(novo->categoria, tempLeitura, novo->tamanhoCategoria);
    }
    strtok(NULL, ",");

    novo->tamanhoRegistro = 36 + novo->tamanhoModelo + novo->tamanhoCategoria; // Contabiliza tamanho do registro

    // Adiciona registro criado à tabela e atualiza informações dessa
    tabela->byteProxReg += novo->tamanhoRegistro;
    tabela->nroRegistros += 1;
    tabela->nroRegRemovidos += !novo->removido;
    tabela->veiculos = realloc(tabela->veiculos, tabela->nroRegistros * sizeof(VEICULO_t*));
    tabela->veiculos[tabela->nroRegistros - 1] = novo;

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

    // Código para tratamento de retorno
    int retorno = 0;

    // Escreve tabela
    retorno = escreveTabelaVeiculos(saida, tabela);

    // Libera memória da tabela
    destroiTabelaVeiculos(tabela);

    // Retorno de sucesso
    return retorno;
}
