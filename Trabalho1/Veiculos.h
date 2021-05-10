#ifndef __VEICULOS_H_
#define __VEICULOS_H_

#include <stdlib.h>

// Estrutura dado do registro de cada veículo (Registro de Dados)
typedef struct {
    char removido,
        prefixo[5],
        data[10];
    int32_t tamanhoRegistro,
        tamanhoModelo,
        tamanhoCategoria,
        quantidadeLugares,
        codLinha;
    char *modelo,
        *categoria;
} VEICULO_t;

// Estrutura de dados da tabela de veículos (Registro de Cabeçalho)
typedef struct {
    char status;
    int64_t byteProxReg;
    int32_t nroRegistros,
        nroRegRemovidos;
    char descrevePrefixo[18],
        descreveData[35],
        descreveLugares[42],
        descreveLinha[17],
        descreveModelo[20],
        descreveCategoria[26];
    VEICULO_t *veiculos;
} VEICULOS_t;

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaVeiculos(char *entrada, char *saida);

/* Adiciona veículo a partir de [registro] à [tabela] já criada */
VEICULO_t *adicionaVeiculo(VEICULOS_t *tabela, char *registro);

#endif // __VEICULOS_H_
