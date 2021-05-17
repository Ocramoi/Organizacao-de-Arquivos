#include "SelectVeiculos.h"

#include <stdio.h>
#include <stdlib.h>

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
        if (removido == '0') {
            controleRemovidos++;
            int32_t offset; fread(&offset, sizeof(int32_t), 1, arq);
            fseek(arq, offset - 5, SEEK_CUR);
            continue;
        }
    }

    free(prefixo); free(data); free(lugares); free(linha); free(modelo); free(categoria);
    return 0;
}
