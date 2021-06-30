#include "ArvoreB.h"

NO_ARVB_t* criaNoArvB() {
    NO_ARVB_t *arv = NULL;
    arv = malloc(sizeof(NO_ARVB_t));
    arv->folha = -1;
    arv->nroChavesIndexadas = 0;
    for (int i = 0; i < (REGS_FOLHA + 1); ++i)
        arv->ponteirosNos[i] = -1;
    for (int i = 0; i < REGS_FOLHA; ++i) {
        arv->registros->chave = -1;
        arv->registros->ponteiroRegistro = -1;
    }
    return arv;
}
