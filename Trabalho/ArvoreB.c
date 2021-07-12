#include "ArvoreB.h"
#include <string.h>

// Definições
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no);
NO_ARVB_t* criaNoArvB();
NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn);
int escreveCabecalhoArvB(ARVB_t *arvore);

// Implementação
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no) {
    if (!arv || !arv->nomeArq || !no || no->rrnNo < 0)
        return 1;

    FILE *arq = fopen(arv->nomeArq, "r+b");
    if (!arq)
        return 1;
    char statusArq; fread(&statusArq, sizeof(char), 1, arq);
    if (statusArq == '0') {
        fclose(arq);
        return 1;
    }
    int32_t noRaiz; fread(&noRaiz, sizeof(int32_t), 1, arq);

    fseek(arq, TAM_PAGINA*(no->rrnNo + 1), SEEK_SET);

    fwrite(&(no->folha), sizeof(char), 1, arq);
    fwrite(&(no->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fwrite(&(no->rrnNo), sizeof(int32_t), 1, arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fwrite(&(no->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].chave), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].ponteiroRegistro), sizeof(int64_t), 1, arq);
    }
    fwrite(&(no->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    /* fseek(arq, 0, SEEK_SET); */
    /* rewind(arq); */
    /* fwrite("1", sizeof(char), 1, arq); */
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

    FILE *arq = fopen(arvore->nomeArq, "rb");

    fseek(arq, TAM_PAGINA*(rrn + 1), SEEK_SET);

    NO_ARVB_t *noTemp = criaNoArvB();
    fread(&(noTemp->folha), sizeof(char), 1, arq);
    fread(&(noTemp->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fread(&(noTemp->rrnNo), sizeof(int32_t), 1, arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fread(&(noTemp->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].chave), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].ponteiroRegistro), sizeof(int64_t), 1, arq);
    }
    fread(&(noTemp->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    fclose(arq);
    return noTemp;
}

int escreveCabecalhoArvB(ARVB_t *arvore) {
    if (!arvore || !arvore->nomeArq)
        return 1;

    FILE *arq = fopen(arvore->nomeArq, "r+b");

    fseek(arq, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arvore->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arvore->proxNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);

    fclose(arq);
    return 0;
}

ARVB_t *criaArvB(char *arquivo) {
    if (!arquivo)
        return NULL;

    ARVB_t *arv = malloc(sizeof(ARVB_t));
    if (!arv)
        return NULL;

    arv->noRaiz = 0;
    arv->proxNo = 1;
    arv->nomeArq = arquivo;

    FILE *arq = fopen(arquivo, "w+b");
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arv->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arv->proxNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);
    fclose(arq);

    NO_ARVB_t *tempNo = criaNoArvB();
    tempNo->folha = '1';
    tempNo->rrnNo = 0;
    escreveNoArvB(arv, tempNo);
    free(tempNo);

    return arv;
}

int splitRrnArvB(ARVB_t *arv,
                 int rrn,
                 int ith) {
    int t = REGS_FOLHA/2;
    NO_ARVB_t *noX = leNoArvB(arv, rrn),
        *noZ = criaNoArvB(),
        *noY = leNoArvB(arv, noX->ponteirosNos[ith]);
    noZ->rrnNo = arv->proxNo;
    arv->proxNo++;

    noZ->folha = noY->folha;
    noZ->nroChavesIndexadas = t - 1;

    for (int j = 0; j < t - 1; ++j) {
        noZ->registros[j].chave = noY->registros[j + t].chave;
        noZ->registros[j].ponteiroRegistro = noY->registros[j + t].ponteiroRegistro;
    }

    if (noY->folha != '1') {
        for (int j = 0; j < t; ++j)
            noZ->ponteirosNos[j] = noY->ponteirosNos[j + t];
    }

    noY->nroChavesIndexadas = t - 1;
    for (int j = noX->nroChavesIndexadas; j >= ith + 1; --j)
        noX->ponteirosNos[j + 1] = noX->ponteirosNos[j];
    noX->ponteirosNos[ith + 1] = noZ->rrnNo;

    for (int j = noX->nroChavesIndexadas - 1; j >= ith; --j) {
        noX->registros[j + 1].chave = noX->registros[j].chave;
        noX->registros[j + 1].ponteiroRegistro = noX->registros[j].ponteiroRegistro;
    }

    noX->registros[ith].chave = noY->registros[t].chave;
    noX->registros[ith].ponteiroRegistro = noY->registros[t].ponteiroRegistro;
    noX->nroChavesIndexadas++;

    int retorno = 0;
    retorno ^= escreveCabecalhoArvB(arv);
    retorno ^= escreveNoArvB(arv, noY);
    retorno ^= escreveNoArvB(arv, noZ);
    retorno ^= escreveNoArvB(arv, noX);

    free(noX); free(noY); free(noZ);
    return retorno;
}

int adicionaRegistroRRNDisponivelArvB(ARVB_t *arv,
                                      int rrn,
                                      int chave,
                                      int64_t offsetRegistro) {
    int ret = 1;
    NO_ARVB_t *tempNo = leNoArvB(arv, rrn);
    if (!tempNo)
        return ret;
    int i = tempNo->nroChavesIndexadas - 1;

    if (tempNo->folha == '1') {
        while (i >= 0 && chave < tempNo->registros[i].chave) {
            tempNo->registros[i + 1].chave = tempNo->registros[i].chave;
            tempNo->registros[i + 1].ponteiroRegistro = tempNo->registros[i].ponteiroRegistro;
            i--;
        }
        tempNo->registros[i + 1].chave = chave;
        tempNo->registros[i + 1].ponteiroRegistro = offsetRegistro;
        tempNo->nroChavesIndexadas++;

        if (escreveNoArvB(arv, tempNo))
            ret = 1;
        else
            ret = 0;
    } else {
        while (i >= 0 && chave < tempNo->registros[i].chave)
            --i;
        i++;

        NO_ARVB_t *noInsercao = leNoArvB(arv, tempNo->ponteirosNos[i]);
        if (noInsercao->nroChavesIndexadas == REGS_FOLHA) {
            free(tempNo); free(noInsercao);
            splitRrnArvB(arv, rrn, i);
            tempNo = leNoArvB(arv, rrn); noInsercao = leNoArvB(arv, tempNo->ponteirosNos[i]);
            if (chave > tempNo->registros[i].chave)
                i++;
            free(noInsercao);
        }
        ret = adicionaRegistroRRNDisponivelArvB(arv, tempNo->ponteirosNos[i], chave, offsetRegistro);
    }
    free(tempNo);
    return ret;
}

int adicionaRegistroArvB(ARVB_t *arvore,
                         int chave,
                         int64_t offsetRegistro) {
    if (!arvore || !arvore->nomeArq)
        return 1;

    /* if (pesquisaArvB(arvore, chave) >= 0) */
    /*     return 1; */

    int retorno;
    NO_ARVB_t *root = leNoArvB(arvore, arvore->noRaiz);
    if (root->nroChavesIndexadas == REGS_FOLHA) {
        NO_ARVB_t *noInsercao = criaNoArvB();
        noInsercao->rrnNo = arvore->proxNo;
        arvore->proxNo++;

        noInsercao->folha = '0';
        noInsercao->nroChavesIndexadas = 0;
        noInsercao->ponteirosNos[0] = arvore->noRaiz;
        arvore->noRaiz = noInsercao->rrnNo;
        escreveNoArvB(arvore, noInsercao);

        escreveCabecalhoArvB(arvore);

        free(noInsercao);
        splitRrnArvB(arvore, arvore->noRaiz, 0);
        retorno = adicionaRegistroRRNDisponivelArvB(arvore, arvore->noRaiz, chave, offsetRegistro);
    } else
        retorno = adicionaRegistroRRNDisponivelArvB(arvore,
                                                    arvore->noRaiz,
                                                    chave,
                                                    offsetRegistro);

    free(root);
    return retorno;
}

ARVB_t *populaArvB(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo)
        return NULL;

    char status;
    fread(&status, sizeof(char), 1, arquivo);
    if (status != '1') {
        fclose(arquivo);
        return NULL;
    }

    ARVB_t *arvore = malloc(sizeof(ARVB_t));
    arvore->nomeArq = nomeArquivo;
    fread(&(arvore->noRaiz), sizeof(int32_t), 1, arquivo);
    fread(&(arvore->proxNo), sizeof(int32_t), 1, arquivo);

    fclose(arquivo);
    return arvore;
}

int64_t pesquisaRRNArvB(FILE *arqArvore,
                        int rrn,
                        int chave) {
    if (rrn == -1)
        return -1;

    fseek(arqArvore, 77*(rrn + 1), SEEK_SET);

    NO_ARVB_t *tempNo = criaNoArvB();
    fread(&(tempNo->folha), sizeof(char), 1, arqArvore);
    fread(&(tempNo->nroChavesIndexadas), sizeof(int32_t), 1, arqArvore);
    fread(&(tempNo->rrnNo), sizeof(int32_t), 1, arqArvore);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fread(&(tempNo->ponteirosNos[i]), sizeof(int32_t), 1, arqArvore);
        fread(&(tempNo->registros[i].chave), sizeof(int32_t), 1, arqArvore);
        fread(&(tempNo->registros[i].ponteiroRegistro), sizeof(int64_t), 1, arqArvore);
    }
    fread(&(tempNo->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arqArvore);

    int pos;
    for (pos = 0; pos < tempNo->nroChavesIndexadas; ++pos) {
        if (tempNo->registros[pos].chave > chave)
            break;
        if (tempNo->registros[pos].chave == chave) {
            int64_t offset = tempNo->registros[pos].ponteiroRegistro;
            free(tempNo);
            return offset;
        }
    }

    int rrnPesq = tempNo->ponteirosNos[pos];
    free(tempNo);
    return pesquisaRRNArvB(arqArvore, rrnPesq, chave);
}

int64_t pesquisaArvB(ARVB_t *arvore, int chave) {
    if (!arvore)
        return -1;

    FILE *arq = fopen(arvore->nomeArq, "rb");
    int64_t offsetReg = pesquisaRRNArvB(arq, arvore->noRaiz, chave);
    fclose(arq);

    return offsetReg;
}
