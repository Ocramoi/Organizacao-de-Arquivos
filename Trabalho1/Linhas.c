#include "Linhas.h"
#include "LeLinha.h"

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

/* "SELECT * from Linhas" -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de linhas */
int selectAllLinhas(char *tabela) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return 1;

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arq);
    if (status != '1') {
        fclose(arq);
        return 1;
    }
    fseek(arq, sizeof(int64_t), SEEK_CUR);

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

    // Lê registro a registro contabilizando removidos
    int controleRemovidos = 0;
    for (int i = 0; i < nroRegs + nroRemvs; ++i) {
        char removido; fread(&removido, sizeof(char), 1, arq);
        int32_t offset; fread(&offset, sizeof(int32_t), 1, arq);
        if (removido == '0') { // Confere se removido, o contabilizando e pulando para o próximo registro
            controleRemovidos++;
            fseek(arq, offset, SEEK_CUR);
            continue;
        }

        // Lê valores brutos do registro e os formata
        int32_t codLinha; fread(&codLinha, sizeof(int32_t), 1, arq);
        char aceitaCartao; fread(&aceitaCartao, sizeof(char), 1, arq);
        int32_t tamNome; fread(&tamNome, sizeof(int32_t), 1, arq);
        char curNome[tamNome + 1]; fread(curNome, sizeof(char), tamNome, arq); curNome[tamNome] = '\0';
        int32_t tamCor; fread(&tamCor, sizeof(int32_t), 1, arq);
        char curCor[tamCor + 1]; fread(curCor, sizeof(char), tamCor, arq); curCor[tamCor] = '\0';

        // Exibe informações no formato indicado
        printf("%s: %d\n", descreveCodigo, codLinha);

        printf("%s: ", descreveNome);
        if (tamNome)
            printf("%s\n", curNome);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", descreveLinha);
        if (tamCor)
            printf("%s\n", curCor);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", descreveCartao);
        switch (aceitaCartao) {
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
    }

    // Libera memória alocada
    free(descreveCodigo); free(descreveCartao); free(descreveNome); free(descreveLinha);
    // Fecha arquivo da tabela
    fclose(arq);

    return 0;
}
