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

ARVB_t *criaArvB(FILE* arquivo) {
    if (!arquivo)
        return NULL;

    ARVB_t *arv = malloc(sizeof(ARVB_t));
    if (!arv)
        return NULL;

    arv->noRaiz = -1;
    arv->numNos = 0;
    arv->arq = arquivo;

    return arv;
}

int adicionaRegistroArvB(ARVB_t *arvore, int chave, int offsetRegistro) {
    NO_ARVB_t *tempNo = criaNoArvB();
    if (arvore->noRaiz == -1) {
        tempNo->folha = '1';
        tempNo->rrnNo = 0;
        arvore->noRaiz = tempNo->rrnNo;
    }

    // Caso inicial -> nó atual raíz
    if (arvore->noRaiz == tempNo->rrnNo) {
        // Caso nó raíz não cheio
        if (tempNo->nroChavesIndexadas < REGS_FOLHA) {
            tempNo->registros[tempNo->nroChavesIndexadas].chave = chave;
            tempNo->registros[tempNo->nroChavesIndexadas].ponteiroRegistro = offsetRegistro;

            tempNo->nroChavesIndexadas++;
        }

    }

    return 0;
}

NO_ARVB_t *noRnnArvB(ARVB_t *arv, int rnn) {
    if (!arv || !arv->arq)
        return NULL;

    long initOffset = ftell(arv->arq);
    fseek(arv->arq, TAM_PAGINA*(rnn + 1), SEEK_SET);

    NO_ARVB_t *noTemp = criaNoArvB();
    fread(&(noTemp->folha), sizeof(char), 1, arv->arq);
    fread(&(noTemp->nroChavesIndexadas), sizeof(int32_t), 1, arv->arq);
    fread(&(noTemp->rrnNo), sizeof(int32_t), 1, arv->arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fread(&(noTemp->ponteirosNos[i]), sizeof(int32_t), 1, arv->arq);
        fread(&(noTemp->registros[i].chave), sizeof(int32_t), 1, arv->arq);
        fread(&(noTemp->registros[i].ponteiroRegistro), sizeof(int32_t), 1, arv->arq);
    }
    fread(&(noTemp->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arv->arq);

    fseek(arv->arq, initOffset, SEEK_SET);
    return noTemp;
}

int escreveNoArvB(ARVB_t *arv, NO_ARVB_t *no) {
    if (!arv || !arv->arq)
        return 1;

    fseek(arv->arq, TAM_PAGINA*(arv->numNos + 1), SEEK_SET);

    fwrite(&(no->folha), sizeof(char), 1, arv->arq);
    fwrite(&(no->nroChavesIndexadas), sizeof(int32_t), 1, arv->arq);
    fwrite(&(no->rrnNo), sizeof(int32_t), 1, arv->arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fwrite(&(no->ponteirosNos[i]), sizeof(int32_t), 1, arv->arq);
        fwrite(&(no->registros[i].chave), sizeof(int32_t), 1, arv->arq);
        fwrite(&(no->registros[i].ponteiroRegistro), sizeof(int32_t), 1, arv->arq);
    }
    fwrite(&(no->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arv->arq);

    return 0;
}
