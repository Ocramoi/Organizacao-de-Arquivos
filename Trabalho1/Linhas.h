#ifndef __LINHAS_H_
#define __LINHAS_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char removido,
        cartao,
        *nomeLinha,
        *corLinha;
    int32_t codLinha;
} LINHA_t;

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaLinhas(char *entrada, char *saida);

/* Adiciona linha a partir de [registro] à [tabela] já criada */
int adicionaLinha(FILE *tabela, char *registro, int64_t *offset);

/* "SELECT * from Linhas" -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de linhas */
int selectAllLinhas(char *tabela);

/* "SELECT * from Linhas WHERE ..." -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de linhas com [campo] de [valor] */
int selectLinhas(char *tabela, char *campo, char *valor);

/* "INSERT INTO Linha ..." -> Insere informações lidas em [registro] na tabela do arquivo [nomeArq] dada */
int insertLinha(char *nomeArq, char *registro);

#endif // __LINHAS_H_
