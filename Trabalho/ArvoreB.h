#ifndef __ARVOREB_H_
#define __ARVOREB_H_

#include <stdlib.h>
#include <stdio.h>

/* Definições da árvore B */
#define TAM_PAGINA 77
#define REGS_FOLHA 4

/* Estrutura de registro (chave + offset de arquivo) */
typedef struct {
    int32_t chave;
    int64_t ponteiroRegistro;
} CONJUNTO_CHAVE_PONTEIRO_t;

/* Estrutura de informações de nó de árvore */
typedef struct {
    char folha;
    int32_t nroChavesIndexadas,
        rrnNo;
    int32_t ponteirosNos[REGS_FOLHA + 1];
    CONJUNTO_CHAVE_PONTEIRO_t registros[REGS_FOLHA];
} NO_ARVB_t;

/* Estrutura de informações da árvore */
typedef struct {
    int64_t noRaiz,
        proxNo;
    char *nomeArq;
} ARVB_t;

/* Popula nova árvore b com a partir de [nomeArquivo] de árvore dado */
ARVB_t *populaArvB(char *nomeArquivo);

/* Adiciona registro com [chave], presente no [offsetRegistro] do arquivo de tabela na [arvore] */
int adicionaRegistroArvB(ARVB_t *arvore,
                         int chave,
                         int64_t offsetRegistro);

/* Pesquisa offset de registro na [arvore] dada a partir da [chave] de pesquisa */
int64_t pesquisaArvB(ARVB_t *arvore, int chave);

#endif // __ARVOREB_H_
