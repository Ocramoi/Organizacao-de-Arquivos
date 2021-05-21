#ifndef __VEICULOSARQ_H_
#define __VEICULOSARQ_H_

#include <stdlib.h>
#include <stdio.h>

/* Adiciona veículo a partir de [registro] ao arquivo [tabela] já criado */
int adicionaVeiculoARQ(FILE *tabela, char *registro, int64_t *offset);

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida);

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int buscaVeiculos(char *entrada, char *nomeDoCampo, char *valor);


#endif // __VEICULOSARQ_H_
