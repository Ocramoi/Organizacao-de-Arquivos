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

/* "CREATE INDEX ... Linhas" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreLinhas(char *arvore, char *tabela);

/* "INSERT INTO Linhas [INDEX] ..." -> Insere informações lidas em [registro] na árvore do arquivo [arqArvore] dada presente no [offsetInsercao] */
int adicionaLinhaArvore(char *arqArvore, char *registro, int64_t offsetInsercao);

/* "SELECT * from Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de linhas com [codLinha] dado com pesquisa na árvore [arqArvore] */
int pesquisaLinhaArvB(char *arqTabela, char *arqArvore, int codLinha);

#endif // __LINHAS_H_
