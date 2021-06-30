#ifndef __ARVOREB_H_
#define __ARVOREB_H_

#define TAM_PAGINA 77
#define REGS_FOLHA 4

#include <stdlib.h>

typedef struct {
    int32_t chave;
    int64_t ponteiroRegistro;
} CONJUNTO_CHAVE_PONTEIRO_t;

typedef struct {
    char folha;
    int32_t nroChavesIndexadas,
        rrnNo;
    int32_t ponteirosNos[REGS_FOLHA + 1];
    CONJUNTO_CHAVE_PONTEIRO_t registros[REGS_FOLHA];
} NO_ARVB_t;

NO_ARVB_t* criaNoArvB();

#endif // __ARVOREB_H_
