#include "TratamentoDeValores.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *trataData(char *original) {
    // Confere se valor não nulo
    if (original[0] == '\0')
        return NULL;

    // Inicializa campos da data
    char dia[3] = {original[8], original[9], '\0'},
        mes[10],
        ano[5] = {original[0], original[1], original[2], original[3], '\0'};
    // Lê mês por extenso do valor numérico
    if (original[0] != 'N') {
        if (original[5] == '0') {
            switch (original[6]) {
                case '1':
                    strcpy(mes, "janeiro");
                    break;
                case '2':
                    strcpy(mes, "fevereiro");
                    break;
                case '3':
                    strcpy(mes, "março");
                    break;
                case '4':
                    strcpy(mes, "abril");
                    break;
                case '5':
                    strcpy(mes, "maio");
                    break;
                case '6':
                    strcpy(mes, "junho");
                    break;
                case '7':
                    strcpy(mes, "julho");
                    break;
                case '8':
                    strcpy(mes, "agosto");
                    break;
                case '9':
                    strcpy(mes, "setembro");
                    break;
            }
        } else {
            switch (original[6]) {
                case '0':
                    strcpy(mes, "outubro");
                    break;
                case '1':
                    strcpy(mes, "novembro");
                    break;
                case '2':
                    strcpy(mes, "dezembro");
                    break;
            }
        }
    }

    // Aloca string de data formatada e copia campos na ordem correta
    char *tratado = calloc((strlen(mes) + 15), sizeof(char));
    sprintf(tratado, "%s de %s de %s", dia, mes, ano);

    return tratado;
}

/* Remove aspas exteriores à string [original] com máximo de [limite] caracteres */
char *trataAspas(char *original, unsigned int limite) {
    char *tratada = malloc(sizeof(char) * limite);
    if (original[0] == '"')
        sscanf(original, "\"%[^\"]", tratada);
    else
        sscanf(original, "%[^\n]", tratada);
    return tratada;
}
