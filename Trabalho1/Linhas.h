#ifndef __LINHAS_H_
#define __LINHAS_H_

#include <stdlib.h>

// Estrutura dado do registro de cada linha (Registro de Dados)
typedef struct {
    int32_t tamanhoRegistro,
        tamanhoNome,
        tamanhoCor;
    char removido,
        codLinha[4],
        aceitaCartao[1],
        nomeLinha[100],
        corLinha[100];
} LINHA_t;

// Estrutura de dados da tabela de linhas (Registro de Cabeçalho)
typedef struct {
    char status;
    int64_t byteProxReg;
    int32_t nroRegistros,
        nroRegRemovidos;
    char descreveCodigo[15],
        descreveCartao[13],
        descreveNome[13],
        descreveLinha[24];
    LINHA_t *linhas;
} LINHAS_t;

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaLinhas(char *entrada, char *saida);

/* Adiciona linha a partir de [registro] à [tabela] já criada */
LINHA_t *adicionaLinha(LINHAS_t *tabela, char *registro);

#endif // __LINHAS_H_
