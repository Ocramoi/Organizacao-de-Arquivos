#ifndef __VEICULOSARQ_H_
#define __VEICULOSARQ_H_

#include <stdlib.h>
#include <stdio.h>

// Estrutura dado do registro de cada veículo (Registro de Dados)
typedef struct {
    char removido,
        prefixo[6],
        dataOriginal[11];
    int32_t quantidadeLugares,
        codLinha,
        tamRegistro;
    char *modelo,
        *categoria,
        *data;
} VEICULO_t;

typedef struct {
    int64_t byteOffset;
    int32_t nroRegs,
        nroRems;
    char status,
        *prefixo,
        *data,
        *lugares,
        *linha,
        *modelo,
        *categoria;
} CABECALHO_VEICULOS_t;

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida);

/* "SELECT * from Veiculos" -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de veículos */
int selectAllVeiculos(char *tabela);

/* "SELECT * from Veiculos WHERE ..." -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de veículos com [campo] de [valor] */
int selectVeiculos(char *tabela, char *campo, char *valor);

/* "INSERT INTO Veiculos ..." -> Insere informações lidas em [registro] na tabela do arquivo [nomeArq] dada */
int insertVeiculo(char *nomeArq, char *registro);

/* Cria e popula estrutura de veículo a partir do ponteiro de arquivo dado */
VEICULO_t *leVeiculo(FILE *arq);

/* Libera memória de estrutura de veículo lida */
int destroiVeiculo (VEICULO_t *veiculo);

/* Exibe campos de [veiculo] com descrição do [cabecalho] */
int exibeDescreveVeiculo(CABECALHO_VEICULOS_t *cabecalho, VEICULO_t *veiculo);

/* "CREATE INDEX ... Veiculos" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreVeiculos(char *arvore, char *tabela);

/* "INSERT INTO Veiculos [INDEX] ..." -> Insere informações lidas em [registro] na árvore do arquivo [arqArvore] dada presente no [offsetInsercao] */
int adicionaVeiculoArvore(char *arqArvore, char *registro, int64_t offsetInsercao);

/* "SELECT * from Veiculos WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int pesquisaVeiculoArvB(char *arqTabela, char *arqArvore, char *prefixo);

/* Lê cabeçalho de veículos do [arquivo] dado */
CABECALHO_VEICULOS_t *leCabecalhoVeiculos(FILE *arquivo);

/* Libera memória de [cabecalho] de veículos lido */
int destroiCabecalhoVeiculos(CABECALHO_VEICULOS_t *cabecalho);

/* "SELECT * from Veiculos JOIN Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int selectJoinVeiculosLinhas(char *arqVeiculos, char *arqLinhas, char *campoVeiculos, char *campoLinha);

/* Ordena veículos do arquivo de entrada [arqEntrada] com base no [campoOrdenacao] e escreve lista para arquivo [arqSaida] */
int ordenaVeiculos(char *arqEntrada, char *arqSaida, char *campoOrdenacao);

#endif // __VEICULOSARQ_H_
