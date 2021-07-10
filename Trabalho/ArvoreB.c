#include "ArvoreB.h"
#include <string.h>

// Definições
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no,
                  char raiz);
NO_ARVB_t* criaNoArvB();
NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn);
/* int adicionaRegistroRRNArvB(ARVB_t *arv, */
/*                             int rrn, */
/*                             int chave, */
/*                             int offsetRegistro); */
int escreveCabecalhoArvB(ARVB_t *arvore,
                         FILE *arqArvore);

// Implementação
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no,
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

    fseek(arq, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arq);
    if (raiz)
        fwrite(&(no->rrnNo), sizeof(int32_t), 1, arq);
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

int escreveCabecalhoArvB(ARVB_t *arvore,
                         FILE *arqArvore) {
    if (!arvore || !arvore->nomeArq)
        return 1;

    FILE *arq = arqArvore;
    if (!arq)
        arq = fopen(arvore->nomeArq, "r+b");

    long initOffset = ftell(arqArvore);

    fseek(arqArvore, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arvore->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arvore->proxNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);

    if (arqArvore)
        fseek(arqArvore, initOffset, SEEK_SET);
    else
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
    escreveNoArvB(arv, tempNo, 1);
    free(tempNo);

    return arv;
}

int splitRnnArvB(ARVB_t *arv,
                 int rrn,
                 int chave,
                 int offsetRegistro) {
    NO_ARVB_t *noPai = leNoArvB(arv, rrn),
        *tempNo = criaNoArvB();
    if (!noPai || !tempNo)
        return 1;
    tempNo->rrnNo = arv->proxNo;
    arv->proxNo++;

    int pos;
    for (pos = 0; pos < tempNo->nroChavesIndexadas; ++pos)
        if (tempNo->registros[pos].chave > chave)
            break;
    NO_ARVB_t *noFilho = leNoArvB(arv, tempNo->ponteirosNos[pos]);
    if (!noFilho)
        return 1;

    tempNo->folha = noFilho->folha;
    tempNo->nroChavesIndexadas = REGS_FOLHA/2;
    for (int i = 0; i < REGS_FOLHA/2; ++i) {
        tempNo->registros[i].chave = noFilho->registros[(REGS_FOLHA/2) + i].chave;
        tempNo->registros[i].ponteiroRegistro = noFilho->registros[(REGS_FOLHA/2) + i].ponteiroRegistro;
    }

    if (noFilho->folha != '1')
        for (int i = 0; i < (REGS_FOLHA/2) + 1; ++i)
            tempNo->ponteirosNos[i] = noFilho->ponteirosNos[i + (REGS_FOLHA/2) + 1];

    noFilho->nroChavesIndexadas = REGS_FOLHA/2;
    for (int  i = noPai->nroChavesIndexadas + 1; i >= pos + 1; --i)
        noPai->ponteirosNos[i + 1] = noPai->ponteirosNos[i];

    noPai->ponteirosNos[pos + 1] = tempNo->rrnNo;
    for (int i = noPai->nroChavesIndexadas; i >= pos; --i) {
        noPai->registros[i + 1].chave = noPai->registros[i].chave;
        noPai->registros[i + 1].ponteiroRegistro = noPai->registros[i].ponteiroRegistro;
    }
    noPai->registros[pos].chave = noPai->registros[REGS_FOLHA/2].chave;
    noPai->registros[pos].ponteiroRegistro = noPai->registros[REGS_FOLHA/2].ponteiroRegistro;

    noPai->nroChavesIndexadas++;

    int retornoErro = (escreveCabecalhoArvB(arv, NULL) ||
                       escreveNoArvB(arv, noFilho, (noFilho->rrnNo == arv->noRaiz)) ||
                       escreveNoArvB(arv, tempNo, (tempNo->rrnNo == arv->noRaiz)) ||
                       escreveNoArvB(arv, noPai, (noPai->rrnNo == arv->noRaiz)));
    free(noFilho); free(noPai); free(tempNo);
    return retornoErro;
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

        if (tempNo->ponteirosNos[pos] == -1) {
            tempNo->ponteirosNos[pos] = arv->proxNo;
            arv->proxNo++;
            escreveCabecalhoArvB(arv, NULL);
            escreveNoArvB(arv, tempNo, 0);
        }

        int prox = tempNo->ponteirosNos[pos];
        free(tempNo);

        int retorno = adicionaRegistroRRNArvB(arv, prox, chave, offsetRegistro);
        if (retorno == -1)
            retorno = splitRnnArvB(arv, rrn, chave, offsetRegistro);
        return retorno;
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

        for (int i = pos + 1; i < REGS_FOLHA; ++i) {
            tempNo->registros[i].chave = registros[i].chave;
            tempNo->registros[i].ponteiroRegistro = registros[i].ponteiroRegistro;
        }

        int ret = 0;
        if (escreveNoArvB(arv, tempNo, (tempNo->rrnNo == arv->noRaiz)))
            ret = 1;

        free(tempNo);
        return ret;
    }

    return -1;
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
    fseek(arquivo, 68, SEEK_CUR);

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
        if (tempNo->registros[pos].chave > chave) {
            break;
        }
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
