#ifndef __JOIN_H_
#define __JOIN_H_

#include "Veiculos.h"
#include "Linhas.h"
#include "ArvoreB.h"

/* "SELECT * from Veiculos JOIN Linhas WHERE ..." -> Seleciona e exibe veículos de [arqVeiculos] e linhas de [arqLinhas] com mesmo código de linha */
int selectJoinVeiculosLinhas(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha);

/* "SELECT * from Veiculos JOIN Linhas WHERE INDEX ..." -> Seleciona e exibe veículos de [arqVeiculos] e linhas de [arqLinhas] com mesmo código de linha, realizando a pesquisa na árvore b */
int selectJoinVeiculosLinhasArvB(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha, char *indiceLinha);

/* Realiza a ordenação e merge dos registros de veículos em [arqVeiculos] e linhas em [arqLinhas] com base no código de linha */
int ordMerge(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha);

#endif // __JOIN_H_
