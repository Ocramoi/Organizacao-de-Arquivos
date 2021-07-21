#ifndef __JOIN_H_
#define __JOIN_H_

#include "Veiculos.h"
#include "Linhas.h"

/* "SELECT * from Veiculos JOIN Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int selectJoinVeiculosLinhas(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha);

#endif // __JOIN_H_
