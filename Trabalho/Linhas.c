#include "Linhas.h"
#include "LeLinha.h"
#include "TratamentoDeValores.h"
#include "ArvoreB.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona linha a partir de [tempRegistro] ao arquivo [tabela] já criado */
int adicionaLinha(FILE *tabela, char *registro, int64_t *offset) {
    // Trata erros de ponteiros
    if (!tabela || !registro || !offset)
        return -1;

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro, captura o código

    // Contabiliza e escreve se registro foi removido e escreve o código nas duas situações
    int controleRemovido;
    if (tempLeitura[0] == '*'){
        memmove(tempLeitura, tempLeitura+1, strlen(tempLeitura));
        controleRemovido = 1;
        fwrite("0", sizeof(char), 1, tabela); //Incrementa o removido com 1, e define 0 no arquivo.
    }
    else{
        controleRemovido = 0;
        fwrite("1", sizeof(char), 1, tabela); //Adiciona 0 aos removidos, e define 1 no arquivo.
    }       
        
    //Captura e armazena os dados do arquivo CSV, fez-se o armazenamento para contabilizar o tamanho do arquivo
    int32_t codigo = atoi(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *AceitaCartao = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *NomeDaLinha = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *CorDaLinha = strdup(tempLeitura);

    //Define-se o tamanho do campo NomeDaLinha
    int32_t controleNomeLinha;
    if (!strcmp(NomeDaLinha, "NULO"))
        controleNomeLinha = 0;
    else
        controleNomeLinha = strlen(NomeDaLinha);

    //Define-se o tamanho do campo CorDaLinha
    int32_t controleCorLinha;
    if (!strcmp(CorDaLinha, "NULO"))
        controleCorLinha = 0;
    else
        controleCorLinha = strlen(CorDaLinha);
    
    //Definição e escrita do tamanho de registro 3*int+1*char = 13 + variável
    int32_t tamanhoRegistro;
    tamanhoRegistro = 13 + controleNomeLinha + controleCorLinha;
    fwrite(&tamanhoRegistro, sizeof(int32_t), 1, tabela);
    
    //Escrita do campo codigo.
    fwrite(&codigo, sizeof(int32_t), 1, tabela);

    //Escrita do campo aceitaCartao com sua possibilidade de ser 0.
    if (!strcmp(AceitaCartao, "NULO"))
        fwrite("\0", sizeof(char), 1, tabela);
    else
        fwrite(AceitaCartao, sizeof(char), 1, tabela); 
    
    //Escrita dos demais campos.
    fwrite(&controleNomeLinha, sizeof(int32_t), 1, tabela);
    fwrite(NomeDaLinha, sizeof(char), controleNomeLinha, tabela);
    fwrite(&controleCorLinha, sizeof(int32_t), 1, tabela);
    fwrite(CorDaLinha, sizeof(char), controleCorLinha, tabela);
    tempLeitura = strtok(NULL, ",");
    
    // Contabiliza tamanho do registro
    *offset += 18 + controleNomeLinha + controleCorLinha;

    // Retorna registro adicionado
    return controleRemovido;
}

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaLinhas(char *entrada, char *saida) {
    // Abre arquivos a partir do nome
    FILE *arqEntrada = fopen(entrada, "r"),
         *arqSaida = fopen(saida, "wb+");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *tempRegistro = leLinha(arqEntrada),
    // Inicia tokenização do csv
    *registro = strtok(tempRegistro, ",");

    // Inicializa cabeçalho
    int64_t byteProxReg = 82;
    int32_t nroRegistros = 0;
    int32_t nroRegRemovidos = 0;
    fwrite("0", sizeof(char), 1, arqSaida);
    fwrite(&byteProxReg, sizeof(int64_t), 1, arqSaida);
    fwrite(&nroRegistros, sizeof(int32_t), 1, arqSaida);
    fwrite(&nroRegRemovidos, sizeof(int32_t), 1, arqSaida);
    fwrite(registro, sizeof(char), 15, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 13, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 13, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 24, arqSaida); 

    // Escreve registros do csv linha a linha
    int32_t nReg = 0, nRemovidos = 0;
    int64_t offset = 82;
    while (tempRegistro) {
        free(tempRegistro);
        tempRegistro = leLinha(arqEntrada);
        if (!tempRegistro) break;

        int r = adicionaLinha(arqSaida, tempRegistro, &offset);
        if (r == -1) {
            printf("Falha no processamento do arquivo.");
            free(tempRegistro);
            fclose(arqEntrada); fclose(arqSaida);
            return 1;
        }
        nRemovidos += r;
        nReg++;
    }
    // Fecha arquivo de entrada
    fclose(arqEntrada);

    // Atualiza cabeçalho
    fseek(arqSaida, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, arqSaida);
    fwrite(&offset, sizeof(int64_t), 1, arqSaida);
    nReg = nReg - nRemovidos;
    fwrite(&nReg, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRemovidos, sizeof(int32_t), 1, arqSaida);

    // Fecha arquivo de saída após escrever
    fclose(arqSaida);
    return 0;
}

/* Libera memória de estrutura de linha lida */
int destroiLinha (LINHA_t *linha) {
    // Confere registro
    if (!linha)
        return 1;

    // Libera memória associada
    free(linha->nomeLinha); free(linha->corLinha);
    free(linha);
    return 0;
}

/* Cria e popula estrutura de linha a partir do ponteiro de arquivo dado */
LINHA_t *leObjLinha(FILE *arq) {
    // Confere arquivo
    if (!arq)
        return NULL;

    // Estrutura de linha
    LINHA_t *linha = malloc(sizeof(LINHA_t));

    char removido; fread(&removido, sizeof(char), 1, arq);
    int32_t offset; fread(&offset, sizeof(int32_t), 1, arq);
    if (removido == '0') { // Confere se removido, o contabilizando e pulando para o próximo registro
        fseek(arq, offset, SEEK_CUR);
        free(linha);
        return NULL;
    }

    // Lê valores brutos do registro e os formata
    fread(&(linha->codLinha), sizeof(int32_t), 1, arq);
    fread(&(linha->cartao), sizeof(char), 1, arq);

    int tamNome; fread(&tamNome, sizeof(int32_t), 1, arq);
    if (tamNome) {
        linha->nomeLinha = calloc(tamNome + 1, sizeof(char));
        fread(linha->nomeLinha, sizeof(char), tamNome, arq);
    } else
        linha->nomeLinha = NULL;

    int tamCor; fread(&tamCor, sizeof(int32_t), 1, arq);
    if (tamCor) {
        linha->corLinha = calloc(tamCor + 1, sizeof(char));
        fread(linha->corLinha, sizeof(char), tamCor, arq);
    } else
        linha->corLinha = NULL;

    // Retorna objeto criado
    return linha;
}

/* "SELECT * from Linhas" -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de linhas */
int selectAllLinhas(char *tabela) {
    return selectLinhas(tabela, NULL, NULL);
}

/* "SELECT * from Linhas WHERE ..." -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de linhas com [campo] de [valor] */
int selectLinhas(char *tabela, char *campo, char *valor) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return 1;

    // Se valor de campo a ser pesquisado dado, requer valor de pesquisa
    if (campo && !valor)
        return 1;

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arq);
    if (status != '1') {
        fclose(arq);
        return 1;
    }
    fseek(arq, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arq);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arq);
    if (nroRegs == 0) {
        fclose(arq);
        return -1;
    }

    // Lê strings de cabeçalho
    char *descreveCodigo = calloc(16, sizeof(char)),
        *descreveCartao = calloc(14, sizeof(char)),
        *descreveNome = calloc(14, sizeof(char)),
        *descreveLinha = calloc(25, sizeof(char));
    fread(descreveCodigo, sizeof(char), 15, arq);
    fread(descreveCartao, sizeof(char), 13, arq);
    fread(descreveNome, sizeof(char), 13, arq);
    fread(descreveLinha, sizeof(char), 24, arq);

    // Cria valor de pesquisa a partir da string dada
    char *strTratado = NULL;
    if (valor)
        strTratado = strtok(valor, "\"");
    int numTratado;
    if (strTratado)
        numTratado = atoi(strTratado);

    // Lê registro a registro contabilizando removidos
    int controleRemovidos = 0;
    for (int i = 0; i < nroRegs + nroRemvs; ++i) {
        // Lê novo objeto
        LINHA_t *atual = leObjLinha(arq);

        // Contabiliza se registro removido
        if (arq && !atual) {
            controleRemovidos++;
            continue;
        }

        // Ignora registros não compatíveis caso feita pesquisa por campo
        if (campo) {
            if (!strcmp(campo, "codLinha") && (atual->codLinha != numTratado)) {
                destroiLinha(atual);
                continue;
            }
            if (!strcmp(campo, "aceitaCartao") && (atual->cartao != strTratado[0])) {
                destroiLinha(atual);
                continue;
            }
            if (!strcmp(campo, "nomeLinha") && strcmp(atual->nomeLinha, strTratado)) {
                destroiLinha(atual);
                continue;
            }
            if (!strcmp(campo, "corLinha") && strcmp(atual->corLinha, strTratado)) {
                destroiLinha(atual);
                continue;
            }
        }

        // Exibe informações no formato indicado
        printf("%s: %d\n", descreveCodigo, atual->codLinha);

        printf("%s: ", descreveNome);
        if (atual->nomeLinha)
            printf("%s\n", atual->nomeLinha);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", descreveLinha);
        if (atual->corLinha)
            printf("%s\n", atual->corLinha);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", descreveCartao);
        switch (atual->cartao) {
            case 'S':
                printf("PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
                break;
            case 'N':
                printf("PAGAMENTO EM CARTAO E DINHEIRO\n");
                break;
            case 'F':
                printf("PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
                break;
        }
        printf("\n");

        // Libera registro criado anteriormente
        destroiLinha(atual);
    }

    // Libera memória alocada
    free(descreveCodigo); free(descreveCartao); free(descreveNome); free(descreveLinha);
    // Fecha arquivo da tabela
    fclose(arq);

    // Confere se número de registros removidos correto
    if (controleRemovidos != nroRemvs)
        return 1;

    return 0;
}

LINHA_t *regParaLinha(char *registro) {
    if (!registro)
        return NULL;

    LINHA_t *tempLinha = malloc(sizeof(LINHA_t));
    if (!tempLinha)
        return NULL;

    // Variáveis de armazenamento dos dados de [registro]
    char tempCodLinha[10],
        tempAceitaCartao[10],
        tempNomeLinha[100],
        tempCorLinha[100],
        resto[250];

    // Leitura dos dados de [registro] e alocação
    sscanf(registro, "%s %[^\n]", tempCodLinha, resto);
    if (resto[0] == '"')
        sscanf(resto, "\"%[^\"]\" %[^\n]", tempAceitaCartao, resto);
    else
        sscanf(resto, "%s %[^\n]", tempAceitaCartao, resto);
    if (resto[0] == '"')
        sscanf(resto, "\"%[^\"]\" %[^\n]", tempNomeLinha, tempCorLinha);
    else
        sscanf(resto, "%s %[^\n]", tempNomeLinha, tempCorLinha);

    // Trata os dados de [registro] inseridos
    char *aceitaCartao = trataAspas(tempAceitaCartao, 10),
        *nomeLinha = trataAspas(tempNomeLinha, 100),
        *corLinha = trataAspas(tempCorLinha, 100);

    if (tempCodLinha[0] == '*') {
        memmove(tempCodLinha, tempCodLinha + 1, strlen(tempCodLinha));
        tempLinha->removido = '0';
    }
    else
        tempLinha->removido = '1';

    int32_t codLinha = atoi(tempCodLinha);

    if (!strcmp(aceitaCartao, "NULO")) {
        aceitaCartao = realloc(aceitaCartao, 1);
        aceitaCartao[0] = '\0';
    }

    if (!strcmp(nomeLinha, "NULO")) {
        nomeLinha = realloc(nomeLinha, 1);
        nomeLinha[0] = '\0';
    }

    if (!strcmp(corLinha, "NULO")) {
        corLinha = realloc(corLinha, 1);
        corLinha[0] = '\0';
    }

    tempLinha->cartao = aceitaCartao[0];
    tempLinha->codLinha = codLinha;
    tempLinha->corLinha = corLinha;
    tempLinha->nomeLinha = nomeLinha;

    return tempLinha;
}

/* "INSERT INTO Linha ..." -> Insere informações lidas em [registro] na tabela do arquivo [nomeArq] dada */
int insertLinha(char *nomeArq, char *registro) {
    // Confere ponteiros passados
    FILE *tabela = fopen(nomeArq, "r+b");
    if (!tabela || !registro || !tabela)
        return 1;

    // Checa o status do arquivo [nomeArq] para o [registro] ser escrito 
    char status; fread(&status, sizeof(char), 1, tabela);
    if (status != '1') {
        fclose(tabela);
        return 1;
    }

    // Altera o status do arquivo para em uso [0] 
    fseek(tabela, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, tabela);
    // Busca os campos armazenados no cabeçalho do arquivo [nomeArq] byte offset [offset] e numeros de registros inseridos e [numReg] removidos [numRegRemovidos]
    int64_t offset; fread(&offset, sizeof(int64_t), 1, tabela);
    int32_t numReg; fread(&numReg, sizeof(int32_t), 1, tabela);
    int32_t numRegRemovidos; fread(&numRegRemovidos, sizeof(int32_t), 1, tabela);

    // Posiciona o arquivo [nomeArq] no byte offset/final do arquivo
    fseek(tabela, 0, SEEK_END);

    // Variával de armazenamento dos dados de [registro]
    LINHA_t *tempLinha = regParaLinha(registro);
    if (tempLinha->removido == '0')
        numRegRemovidos += 1;
    else
        numReg += 1;
    fwrite(&(tempLinha->removido), sizeof(char), 1, tabela); // Incrementa registros em 1, e define 1 no arquivo.

    // Escrita dos dados de [registro] no arquivo [nomeArq] já posicionado no final [offset]
    int32_t tamNome = strlen(tempLinha->nomeLinha),
        tamCor = strlen(tempLinha->corLinha),
        tam = 13 + tamNome + tamCor;
    fwrite(&tam, sizeof(int32_t), 1, tabela);
    fwrite(&(tempLinha->codLinha), sizeof(int32_t), 1, tabela);
    fwrite(&(tempLinha->cartao), sizeof(char), 1, tabela);
    fwrite(&tamNome, sizeof(int32_t), 1, tabela);
    fwrite(&(tempLinha->nomeLinha), sizeof(char), tamNome, tabela);
    fwrite(&tamCor, sizeof(int32_t), 1, tabela);
    fwrite(&(tempLinha->corLinha), sizeof(char), tamCor, tabela);

    // Atualização do número de registros [numReg], byte offset [offset]
    int64_t proxReg = offset + tam + 5;
    fseek(tabela, 1, SEEK_SET);
    fwrite(&proxReg, sizeof(int64_t), 1, tabela);
    fwrite(&numReg, sizeof(int32_t), 1, tabela);
    fwrite(&numRegRemovidos, sizeof(int32_t), 1, tabela);

    // Atualização do status do arquivo [nomeArq]
    fseek(tabela, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, tabela);

    // Libera memória alocada
    free(tempLinha);

    // Fecha o arquivo
    fclose(tabela);
    return 0;
}

// Lê e exibe linha lida na [tabela] no [offset]
int exibeLinhaOffset(char *tabela, int64_t offset) {
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return -1;

    // Vai ao offset e lê veículo
    fseek(arq, offset, SEEK_SET);
    LINHA_t *tempLinha = leObjLinha(arq);

    // Lê cabeçalho de arquivo
    fseek(arq, 0, SEEK_SET);

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arq);
    if (status != '1') {
        fclose(arq);
        return 1;
    }
    fseek(arq, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arq);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arq);
    if (nroRegs == 0) {
        fclose(arq);
        return -1;
    }

    // Lê strings de cabeçalho
    char *descreveCodigo = calloc(16, sizeof(char)),
        *descreveCartao = calloc(14, sizeof(char)),
        *descreveNome = calloc(14, sizeof(char)),
        *descreveLinha = calloc(25, sizeof(char));
    fread(descreveCodigo, sizeof(char), 15, arq);
    fread(descreveCartao, sizeof(char), 13, arq);
    fread(descreveNome, sizeof(char), 13, arq);
    fread(descreveLinha, sizeof(char), 24, arq);

    // Exibe informações no formato indicado
    printf("%s: %d\n", descreveCodigo, tempLinha->codLinha);

    printf("%s: ", descreveNome);
    if (tempLinha->nomeLinha)
        printf("%s\n", tempLinha->nomeLinha);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", descreveLinha);
    if (tempLinha->corLinha)
        printf("%s\n", tempLinha->corLinha);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", descreveCartao);
    switch (tempLinha->cartao) {
        case 'S':
            printf("PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR\n");
            break;
        case 'N':
            printf("PAGAMENTO EM CARTAO E DINHEIRO\n");
            break;
        case 'F':
            printf("PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n");
            break;
    }
    printf("\n");

    // Libera memória alocada
    free(descreveCodigo); free(descreveCartao); free(descreveNome); free(descreveLinha);
    // Fecha arquivo da tabela
    fclose(arq);
    // Libera registro criado anteriormente
    destroiLinha(tempLinha);

    return 0;
}

/* "SELECT * from Linhas WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de linhas com [codLinha] dado com pesquisa na árvore [arqArvore] */
int pesquisaLinhaArvB(char *arqTabela, char *arqArvore, int codLinha) {
    // Confere parâmetros
    if (!arqTabela || !arqArvore || codLinha < 0)
        return -1;

    // Popula estrutura da árvore para pesquisa
    ARVB_t *arvore = populaArvB(arqArvore);
    if (!arvore)
        return 1;

    // Busca offset para codLinha dado
    int64_t offSetPesquisa = pesquisaArvB(arvore, codLinha);

    // Caso registro achado na árvore, exibe
    if (offSetPesquisa > 0)
        exibeLinhaOffset(arqTabela, offSetPesquisa);

    // Libera memória alocada
    free(arvore);

    // Trata valor de retorno
    if (offSetPesquisa > 0)
        return 0;
    return 2;
}

/* "CREATE INDEX ... Linhas" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreLinhas(char *tabela, char *arvore) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arqTabela = fopen(tabela, "rb");
    if (!tabela || !arqTabela)
        return 1;

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arqTabela);
    if (status != '1') {
        fclose(arqTabela);
        return 1;
    }
    fseek(arqTabela, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arqTabela);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arqTabela);
    if (nroRegs == 0) {
        fclose(arqTabela);
        return -1;
    }
    fseek(arqTabela, 82, SEEK_SET);

    // Cria e popula árvore
    ARVB_t *arvoreLinhas = populaArvB(arvore);

    // Auxiliar de retorno
    int ret;

    // Para cada registro
    for (int i = 0; i < nroRegs + nroRemvs; ++i) {
        // Salva offset para adição na árvore
        int64_t offsetAtual = ftell(arqTabela);

        // Lê linha atual da lista
        LINHA_t *linhaAtual = leObjLinha(arqTabela);
        if (!linhaAtual) {
            fclose(arqTabela);
            return 1;
        }
        // Confere linha removida
        if (!linhaAtual->removido) {
            destroiLinha(linhaAtual);
            continue;
        }

        // Adiciona linha à árvore
        ret = adicionaRegistroArvB(arvoreLinhas, linhaAtual->codLinha, offsetAtual);
        destroiLinha(linhaAtual);
    }

    // Libera memória alocada
    free(arvoreLinhas); fclose(arqTabela);
    return ret;
}

/* "INSERT INTO Linhas [INDEX] ..." -> Insere informações lidas em [registro] na árvore do arquivo [arqArvore] dada presente no [offsetInsercao] */
int adicionaLinhaArvore(char *arqArvore, char *registro, int64_t offsetInsercao) {
    // Popula árvore com arquivo dado
    ARVB_t *arvore = populaArvB(arqArvore);
    if (!arvore || !arqArvore || !registro || offsetInsercao < 0)
        return 1;

    // Gera linha pelo registro
    LINHA_t *tempLinha = regParaLinha(registro);

    // Confere linha criada
    if (!tempLinha)
        return 1;
    // Adiciona e registra retorno
    int ret = adicionaRegistroArvB(arvore, tempLinha->codLinha, offsetInsercao);

    // Libera memória alocada
    free(arvore); destroiLinha(tempLinha);

    return ret;
}
