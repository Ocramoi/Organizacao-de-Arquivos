#ifndef __LINHAS_H_
#define __LINHAS_H_

#include <stdlib.h>
#include <stdio.h>

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaLinhas(char *entrada, char *saida);

/* Adiciona linha a partir de [registro] à [tabela] já criada */
int adicionaLinha(FILE *tabela, char *registro, int64_t *offset);

#endif // __LINHAS_H_
