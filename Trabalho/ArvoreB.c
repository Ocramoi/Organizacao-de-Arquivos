#include "ArvoreB.h"
#include <string.h>

// Definições
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no,
                  int32_t rrn,
                  char raiz);
NO_ARVB_t* criaNoArvB();
NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn);
int adicionaRegistroRRNArvB(ARVB_t *arv,
                            int rrn,
                            int chave,
                            int offsetRegistro);

// Implementação
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no,
                  int32_t rrn,
                  char raiz) {
    if (!arv || !arv->nomeArq)
        return 1;

    FILE *arq = fopen(arv->nomeArq, "r+b");
    char statusArq; fread(&statusArq, sizeof(char), 1, arq);
    if (statusArq == '0') {
        fclose(arq);
        return 1;
    }
    int32_t noRaiz; fread(&noRaiz, sizeof(int32_t), 1, arq);
    int32_t rrnProxNo; fread(&rrnProxNo, sizeof(int32_t), 1, arq);
    rrnProxNo++;

    fseek(arq, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, arq);

    fseek(arq, TAM_PAGINA*(rrn + 1), SEEK_SET);

    fwrite(&(no->folha), sizeof(char), 1, arq);
    fwrite(&(no->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fwrite(&(no->rrnNo), sizeof(int32_t), 1, arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fwrite(&(no->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].chave), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].ponteiroRegistro), sizeof(int32_t), 1, arq);
    }
    fwrite(&(no->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    fseek(arq, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arq);
    if (raiz)
        fwrite(&rrn, sizeof(int32_t), 1, arq);
    else
        fwrite(&noRaiz, sizeof(int32_t), 1, arq);
    fwrite(&rrnProxNo, sizeof(int32_t), 1, arq);
    fclose(arq);

    return 0;
}

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

NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn) {
    if (!arvore || !arvore->nomeArq)
        return NULL;

    FILE *arq = fopen(arvore->nomeArq, "r");

    fseek(arq, TAM_PAGINA*(rrn + 1), SEEK_SET);

    NO_ARVB_t *noTemp = criaNoArvB();
    fread(&(noTemp->folha), sizeof(char), 1, arq);
    fread(&(noTemp->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fread(&(noTemp->rrnNo), sizeof(int32_t), 1, arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fread(&(noTemp->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].chave), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].ponteiroRegistro), sizeof(int32_t), 1, arq);
    }
    fread(&(noTemp->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    fclose(arq);
    return noTemp;
}

ARVB_t *criaArvB(char *arquivo) {
    if (!arquivo)
        return NULL;

    ARVB_t *arv = malloc(sizeof(ARVB_t));
    if (!arv)
        return NULL;

    arv->noRaiz = 0;
    arv->numNos = 1;
    arv->nomeArq = arquivo;

    FILE *arq = fopen(arquivo, "w+");
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arv->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arv->noRaiz), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);
    fclose(arq);

    NO_ARVB_t *tempNo = criaNoArvB();
    tempNo->folha = '1';
    escreveNoArvB(arv, tempNo, 0, 1);
    free(tempNo);

    return arv;
}

int adicionaRegistroArvB(ARVB_t *arvore,
                         int chave,
                         int offsetRegistro) {
    if (!arvore)
        return 1;

    adicionaRegistroRRNArvB(arvore,
                            arvore->noRaiz,
                            chave,
                            offsetRegistro);
    return 0;
}

int adicionaRegistroRRNArvB(ARVB_t *arv,
                            int rrn,
                            int chave,
                            int offsetRegistro) {
    NO_ARVB_t *tempNo = leNoArvB(arv, rrn);
    if (tempNo->folha != '1') {
        int pos;
        for (pos = 0; pos < tempNo->nroChavesIndexadas; ++pos)
            if (tempNo->registros[pos].chave > chave)
                break;
        free(tempNo);
        return adicionaRegistroRRNArvB(arv, pos, chave, offsetRegistro);
    }

    if (tempNo->nroChavesIndexadas < REGS_FOLHA) {
        int pos;
        for (pos = 0; pos < tempNo->nroChavesIndexadas; ++pos)
            if (tempNo->registros[pos].chave > chave)
                break;

        CONJUNTO_CHAVE_PONTEIRO_t registros[REGS_FOLHA];
        memcpy(registros, tempNo->registros, REGS_FOLHA*sizeof(CONJUNTO_CHAVE_PONTEIRO_t));
        for (int i = 0; i < pos; ++i) {
            tempNo->registros[i].chave = registros[i].chave;
            tempNo->registros[i].ponteiroRegistro = registros[i].ponteiroRegistro;
        }

        tempNo->registros[pos].chave = chave;
        tempNo->registros[pos].ponteiroRegistro = offsetRegistro;

        for (int i = 0; i < (REGS_FOLHA - pos); ++i) {
            tempNo->registros[pos + i].chave = registros[pos + i].chave;
            tempNo->registros[pos + i].ponteiroRegistro = registros[pos + i].ponteiroRegistro;
        }

        escreveNoArvB(arv, tempNo, tempNo->rrnNo, (tempNo->rrnNo == arv->noRaiz));
    }

    free(tempNo);
    return 0;
}
