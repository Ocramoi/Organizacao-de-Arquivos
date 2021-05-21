#include "SelectVeiculos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TratamentoDeValores.h"

/* Seleciona e exibe todos os registros do arquivo binário [tabela] de veículos */
int selectAllVeiculos(char *tabela) {
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return 1;

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

    char *prefixo = calloc(19, sizeof(char)),
        *data = calloc(36, sizeof(char)),
        *lugares = calloc(43, sizeof(char)),
        *linha = calloc(27, sizeof(char)),
        *modelo = calloc(18, sizeof(char)),
        *categoria = calloc(21, sizeof(char));
    fread(prefixo, sizeof(char), 18, arq);
    fread(data, sizeof(char), 35, arq);
    fread(lugares, sizeof(char), 42, arq);
    fread(linha, sizeof(char), 26, arq);
    fread(modelo, sizeof(char), 17, arq);
    fread(categoria, sizeof(char), 20, arq);

    int controleRemovidos = 0;
    for (int i = 0; i < nroRegs; ++i) {
        char removido; fread(&removido, sizeof(char), 1, arq);
        int32_t offset; fread(&offset, sizeof(int32_t), 1, arq);
        if (removido == '0') {
            controleRemovidos++;
            fseek(arq, offset, SEEK_CUR);
            continue;
        }
        char curPrefixo[6]; fread(curPrefixo, sizeof(char), 5, arq); curPrefixo[5] = '\0';
        char curData[11]; fread(curData, sizeof(char), 10, arq); curData[10] = '\0';
        int qntLugares; fread(&qntLugares, sizeof(int32_t), 1, arq);
        int codLinha; fread(&codLinha, sizeof(int32_t), 1, arq);
        int tamModelo; fread(&tamModelo, sizeof(int32_t), 1, arq);
        char curModelo[tamModelo + 1]; fread(curModelo, sizeof(char), tamModelo, arq); curModelo[tamModelo] = '\0';
        int tamCateg; fread(&tamCateg, sizeof(int32_t), 1, arq);
        char curCateg[tamCateg + 1]; fread(curCateg, sizeof(char), tamCateg, arq); curCateg[tamCateg] = '\0';

        printf("%s: %s\n", prefixo, curPrefixo);

        printf("%s: ", modelo);
        if (tamModelo)
            printf("%s\n", curModelo);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", categoria);
        if (tamCateg)
            printf("%s\n", curCateg);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", data);
        char *dataTratada = trataData(curData);
        if (dataTratada) {
            printf("%s\n", dataTratada);
        }
        else
            printf("campo com valor nulo\n");
        free(dataTratada);

        printf("%s: ", lugares);
        if (qntLugares != -1)
            printf("%d\n\n", qntLugares);
        else
            printf("campo com valor nulo\n\n");
    }

    free(prefixo); free(data); free(lugares); free(linha); free(modelo); free(categoria);
    fclose(arq);
    return 0;
}
