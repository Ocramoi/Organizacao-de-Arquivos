#include "ArvoreB.h"

// Definições de função
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no);
NO_ARVB_t* criaNoArvB();
NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn);
int escreveCabecalhoArvB(ARVB_t *arvore);
ARVB_t *criaArvB(char *arquivo);
int splitRrnArvB(ARVB_t *arv,
                 int rrn,
                 int ith,
                 int chave,
                 int64_t offsetRegistro);
int adicionaRegistroRRNDisponivelArvB(ARVB_t *arv,
                                      int rrn,
                                      int chave,
                                      int64_t offsetRegistro);
int adicionaRegistroArvB(ARVB_t *arvore,
                         int chave,
                         int64_t offsetRegistro);
ARVB_t *populaArvB(char *nomeArquivo);
int64_t pesquisaRRNArvB(FILE *arqArvore,
                        int rrn,
                        int chave);
int64_t pesquisaArvB(ARVB_t *arvore, int chave);

// Implementação

/* Escreve [no] dado na posição correta do arquivo de [arv] */
int escreveNoArvB(ARVB_t *arv,
                  NO_ARVB_t *no) {
    // Confere parâmetros
    if (!arv || !arv->nomeArq || !no || no->rrnNo < 0)
        return 1;

    // Abre arquivo
    FILE *arq = fopen(arv->nomeArq, "r+b");
    if (!arq)
        return 1;

    // Lê informações de cabeçalho
    char statusArq; fread(&statusArq, sizeof(char), 1, arq);
    if (statusArq == '0') {
        fclose(arq);
        return 1;
    }
    int32_t noRaiz; fread(&noRaiz, sizeof(int32_t), 1, arq);
    int32_t proxNo; fread(&proxNo, sizeof(int32_t), 1, arq);

    // Caminha para a posição do RRN no arquivo
    fseek(arq, TAM_PAGINA*(no->rrnNo + 1), SEEK_SET);

    // Escreve informações do nó no arquivo
    fwrite(&(no->folha), sizeof(char), 1, arq);
    fwrite(&(no->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fwrite(&(no->rrnNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < REGS_FOLHA; ++i) {
        fwrite(&(no->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].chave), sizeof(int32_t), 1, arq);
        fwrite(&(no->registros[i].ponteiroRegistro), sizeof(int64_t), 1, arq);
    }
    fwrite(&(no->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    // Fecha arquivo
    fclose(arq);

    return 0;
}

/* Cria nó de árvore b vazio */
NO_ARVB_t* criaNoArvB() {
    // Aloca novo nó
    NO_ARVB_t *arv = NULL;
    arv = malloc(sizeof(NO_ARVB_t));

    // Popula valores nulos
    arv->folha = -1;
    arv->nroChavesIndexadas = 0;
    for (int i = 0; i < (REGS_FOLHA + 1); ++i)
        arv->ponteirosNos[i] = -1;
    for (int i = 0; i < REGS_FOLHA; ++i) {
        arv->registros[i].chave = -1;
        arv->registros[i].ponteiroRegistro = -1;
    }

    return arv;
}

// Lê novo nó de [arvore] b presente no [rrn] do arquivo
NO_ARVB_t *leNoArvB(ARVB_t *arvore, int rrn) {
    // Confere parâmetros
    if (!arvore || !arvore->nomeArq || rrn < 0)
        return NULL;

    // Abre arquivo de árvore b
    FILE *arq = fopen(arvore->nomeArq, "rb");

    // Move cursor para início do registro
    fseek(arq, TAM_PAGINA*(rrn + 1), SEEK_SET);

    // Cria novo nó vazio
    NO_ARVB_t *noTemp = criaNoArvB();

    // Lê valroes de nó
    fread(&(noTemp->folha), sizeof(char), 1, arq);
    fread(&(noTemp->nroChavesIndexadas), sizeof(int32_t), 1, arq);
    fread(&(noTemp->rrnNo), sizeof(int32_t), 1, arq);

    for (int i = 0; i < REGS_FOLHA; ++i) {
        fread(&(noTemp->ponteirosNos[i]), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].chave), sizeof(int32_t), 1, arq);
        fread(&(noTemp->registros[i].ponteiroRegistro), sizeof(int64_t), 1, arq);
    }
    fread(&(noTemp->ponteirosNos[REGS_FOLHA]), sizeof(int32_t), 1, arq);

    // Fecha arquivo
    fclose(arq);
    return noTemp;
}

/* (Re)Escreve cabeçalho da [arvore] no seu arquivo */
int escreveCabecalhoArvB(ARVB_t *arvore) {
    // Confere árvore
    if (!arvore || !arvore->nomeArq)
        return 1;

    // Abre arquivo
    FILE *arq = fopen(arvore->nomeArq, "r+b");

    // Escreve informações
    fseek(arq, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arvore->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arvore->proxNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);

    // Fecha arquivo
    fclose(arq);
    return 0;
}

// Cria árvore b vazia com nome de [arquivo]
ARVB_t *criaArvB(char *arquivo) {
    // Confere parâmetro
    if (!arquivo)
        return NULL;

    // Aloca memória da estrutura de árvore
    ARVB_t *arv = malloc(sizeof(ARVB_t));
    if (!arv)
        return NULL;

    // Inicializa valores
    arv->noRaiz = 0;
    arv->proxNo = 1;
    arv->nomeArq = arquivo;

    // Escreve valores da árvore para o arquivo
    FILE *arq = fopen(arquivo, "w+b");
    fwrite("1", sizeof(char), 1, arq);
    fwrite(&(arv->noRaiz), sizeof(int32_t), 1, arq);
    fwrite(&(arv->proxNo), sizeof(int32_t), 1, arq);
    for (int i = 0; i < 68; ++i)
        fwrite("@", sizeof(char), 1, arq);
    // Fecha arquivo
    fclose(arq);

    // Cria e escreve nó vazio de raíz
    NO_ARVB_t *tempNo = criaNoArvB();
    tempNo->folha = '1';
    tempNo->rrnNo = 0;
    escreveNoArvB(arv, tempNo);
    free(tempNo);

    return arv;
}

/* Quebra registro em [rrn] da [arv] no nó filho [ith], inserindo registro ([chave] + [offsetRegistro]) */
int splitRrnArvB(ARVB_t *arv,
                 int rrn,
                 int ith,
                 int chave,
                 int64_t offsetRegistro) {
    // Variável auxiliar de tamanho
    int t = REGS_FOLHA/2;

    // Carrega e cria nós para manipulação
    NO_ARVB_t *noX = leNoArvB(arv, rrn),
        *noZ = criaNoArvB(),
        *noY = leNoArvB(arv, noX->ponteirosNos[ith]);
    noZ->rrnNo = arv->proxNo;
    arv->proxNo++;
    noZ->folha = noY->folha;

    // Carrega registros para divisão
    CONJUNTO_CHAVE_PONTEIRO_t registros[REGS_FOLHA + 1];
    int pos;
    for (pos = 0; pos < REGS_FOLHA && noY->registros[pos].chave < chave; ++pos) {
        registros[pos].chave = noY->registros[pos].chave;
        registros[pos].ponteiroRegistro = noY->registros[pos].ponteiroRegistro;
    }

    for (int i = REGS_FOLHA; i > pos; --i) {
        registros[i].chave = noY->registros[i - 1].chave;
        registros[i].ponteiroRegistro = noY->registros[i - 1].ponteiroRegistro;
    }

    registros[pos].chave = chave;
    registros[pos].ponteiroRegistro = offsetRegistro;

    // Estabelece valores de filhos
    noZ->nroChavesIndexadas = noY->nroChavesIndexadas = t;

    // Atualiza informações de filhos
    for (int i = 0; i < t; ++i) {
        noY->registros[i].chave = registros[i].chave;
        noY->registros[i].ponteiroRegistro = registros[i].ponteiroRegistro;
    }

    for (int i = 0; i < noZ->nroChavesIndexadas; ++i) {
        noZ->registros[i].chave = registros[t + 1 + i].chave;
        noZ->registros[i].ponteiroRegistro = registros[t + 1 + i].ponteiroRegistro;
    }

    // Adiciona valor que "subiu" no nó de rrn
    for (pos = 0; pos < noX->nroChavesIndexadas && noX->registros[pos].chave < chave; ++pos) {
        noX->registros[pos].chave = noX->registros[pos].chave;
        noX->registros[pos].ponteiroRegistro = noX->registros[pos].ponteiroRegistro;
    }

    for (int j = noX->nroChavesIndexadas; j > pos; --j) {
        noX->registros[j].chave = noX->registros[j - 1].chave;
        noX->registros[j].ponteiroRegistro = noX->registros[j - 1].ponteiroRegistro;
    }

    noX->registros[pos].chave = registros[t].chave;
    noX->registros[pos].ponteiroRegistro = registros[t].ponteiroRegistro;

    noX->nroChavesIndexadas++;

    // Atualiza ponteiros dos filhos
    for (int j = 0; j < noZ->nroChavesIndexadas + 1; ++j)
        noZ->ponteirosNos[j] = noY->ponteirosNos[j + t];

    for (int j = noX->nroChavesIndexadas; j >= ith + 1; --j)
        noX->ponteirosNos[j + 1] = noX->ponteirosNos[j];
    noX->ponteirosNos[ith + 1] = noZ->rrnNo;

    // Escreve valores alterados
    int retorno = 0;
    retorno ^= escreveCabecalhoArvB(arv);
    retorno ^= escreveNoArvB(arv, noY);
    retorno ^= escreveNoArvB(arv, noZ);
    retorno ^= escreveNoArvB(arv, noX);

    free(noX); free(noY); free(noZ);
    return retorno;
}

/* Adiciona registro ([chave] + [offsetRegistro]) da [arv] no nó de [rrn] */
int adicionaRegistroRRNDisponivelArvB(ARVB_t *arv,
                                      int rrn,
                                      int chave,
                                      int64_t offsetRegistro) {
    // Auxiliar de retorno
    int ret;
    // Lê nó dado
    NO_ARVB_t *tempNo = leNoArvB(arv, rrn);
    if (!tempNo)
        return ret;
    // Auxiliar de contagem
    int i = tempNo->nroChavesIndexadas - 1;

    // Caso folha alcançada
    if (tempNo->folha == '1') {
        // Adiciona registro na posição correta
        while (i >= 0 && chave < tempNo->registros[i].chave) {
            tempNo->registros[i + 1].chave = tempNo->registros[i].chave;
            tempNo->registros[i + 1].ponteiroRegistro = tempNo->registros[i].ponteiroRegistro;
            i--;
        }
        tempNo->registros[i + 1].chave = chave;
        tempNo->registros[i + 1].ponteiroRegistro = offsetRegistro;
        tempNo->nroChavesIndexadas++;

        // Escreve nó dado
        if (escreveNoArvB(arv, tempNo))
            ret = 1;
        else
            ret = 0;
    } else {
        // Procura ponteiro para alcançar folha correta
        while (i >= 0 && chave < tempNo->registros[i].chave)
            --i;
        i++;

        // Lê nó para possível adição
        NO_ARVB_t *noInsercao = leNoArvB(arv, tempNo->ponteirosNos[i]);
        // caso split necessário
        if (noInsercao->nroChavesIndexadas == REGS_FOLHA && noInsercao->folha == '1')
            ret = splitRrnArvB(arv, rrn, i, chave, offsetRegistro);
        else
            ret = adicionaRegistroRRNDisponivelArvB(arv, tempNo->ponteirosNos[i], chave, offsetRegistro);
        free(noInsercao);
    }
    free(tempNo);

    return ret;
}

/* Adiciona registro com [chave], presente no [offsetRegistro] do arquivo de tabela na [arvore] */
int adicionaRegistroArvB(ARVB_t *arvore,
                         int chave,
                         int64_t offsetRegistro) {
    // Confere parâmetros
    if (!arvore || !arvore->nomeArq || offsetRegistro < 0)
        return 1;

    // Auxiliar de retorno
    int retorno;

    // Lê raíz de árvore
    NO_ARVB_t *root = leNoArvB(arvore, arvore->noRaiz);
    // Caso raíz cheia
    if (root->nroChavesIndexadas == REGS_FOLHA) {
        // Quebra raíz e para inserção no nó correto
        NO_ARVB_t *noInsercao = criaNoArvB();
        noInsercao->rrnNo = arvore->proxNo;
        arvore->proxNo++;

        noInsercao->folha = '0';
        noInsercao->nroChavesIndexadas = 0;
        noInsercao->ponteirosNos[0] = arvore->noRaiz;
        arvore->noRaiz = noInsercao->rrnNo;
        escreveCabecalhoArvB(arvore);
        escreveNoArvB(arvore, noInsercao);

        free(noInsercao);
    }

    // Adiciona registro no nó de raíz
    retorno = adicionaRegistroRRNDisponivelArvB(arvore,
                                                arvore->noRaiz,
                                                chave,
                                                offsetRegistro);

    free(root);
    return retorno;
}

/* Popula nova árvore b com a partir de [nomeArquivo] de árvore dado */
ARVB_t *populaArvB(char *nomeArquivo) {
    // Confere parâmetro e abertura de arquivo
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo || !nomeArquivo)
        return NULL;

    // Confere integridade de árvore
    char status;
    fread(&status, sizeof(char), 1, arquivo);
    if (status != '1') {
        fclose(arquivo);
        return NULL;
    }

    // Cria estrutura de árvore e carrega informações
    ARVB_t *arvore = malloc(sizeof(ARVB_t));
    arvore->nomeArq = nomeArquivo;
    fread(&(arvore->noRaiz), sizeof(int32_t), 1, arquivo);
    fread(&(arvore->proxNo), sizeof(int32_t), 1, arquivo);

    fclose(arquivo);
    return arvore;
}

/* Pesquisa [chave] a partir do nó em [rrn] na árvore [arqArvore] */
int64_t pesquisaRRNArvB(FILE *arqArvore,
                        int rrn,
                        int chave) {
    // Confere parâmetros
    if (rrn == -1)
        return -1;

    // Carrega nó de rrn
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

    // Procura nó correto
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

    // Continua pesquisa recursiva caso necessário
    int rrnPesq = tempNo->ponteirosNos[pos];
    free(tempNo);
    return pesquisaRRNArvB(arqArvore, rrnPesq, chave);
}

/* Pesquisa [chave] na [arvore] */
int64_t pesquisaArvB(ARVB_t *arvore, int chave) {
    // Confere parâmetro
    if (!arvore)
        return -1;

    // Abre arquivo preamptivamente para minimizar abertura de arquivo
    FILE *arq = fopen(arvore->nomeArq, "rb");

    // Pesquisa offset
    int64_t offsetReg = pesquisaRRNArvB(arq, arvore->noRaiz, chave);

    // Fecha arquivo
    fclose(arq);

    return offsetReg;
}
