#ifndef __VEICULOSARQ_H_
#define __VEICULOSARQ_H_

#include <stdlib.h>
#include <stdio.h>

// Estrutura dado do registro de cada veículo (Registro de Dados)
typedef struct {
    char removido,
        prefixo[6];
    int32_t quantidadeLugares,
        codLinha;
    char *modelo,
        *categoria,
        *data;
} VEICULO_t;

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida);

/* "SELECT * from Veiculos" -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de veículos */
int selectAllVeiculos(char *tabela);

/* "SELECT * from Veiculos WHERE ..." -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de veículos com [campo] de [valor] */
int selectVeiculos(char *tabela, char *campo, char *valor);

/* "INSERT INTO Veiculos ..." -> Insere informações lidas em [registro] na tabela do arquivo [nomeArq] dada */
int insertVeiculo(char *nomeArq, char *registro);

/* "CREATE INDEX ... Veiculos" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreVeiculos(char *arvore, char *tabela);

#endif // __VEICULOSARQ_H_
