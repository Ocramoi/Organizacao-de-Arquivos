#include "RadixSort.h"

#include <string.h>

// Base do radix
#define BASE 256

// Radix sort implementado com counting sort na base 256 com tipos genéricos de unsigned de até 8 bytes
void radixSort(void **elemns, size_t tamElem, int nElems, RetornaChave_f (*retonaChave)(void*)) {
    int count[BASE] = {0}, // Vetor de contagem
        pos[BASE]; // Vetor de posição para elementos
    void **cop = malloc(nElems*sizeof(void*)); // Vetor de cópia para tipo genérico
    for (int shift = 0; shift < (int) tamElem; ++shift) { // Loop para shift entre bytes dependente do tamanho
        for (int i = 0; i < nElems; ++i) { // Percorre elementos
            short dig = (retonaChave(elemns[i]) >> shift*8) & 255; // Lê atual byte para ordenação
            count[dig]++; // Atualiza contagem do byte lido
            cop[i] = elemns[i]; // Copia (endereço do) elemento
        }
        pos[0] = 0; // Inicializa posição de dígito
        for (int i = 1; i < 256; ++i) { // Percorre vetor de possíveis bytes
            // Atualiza posição pela contagem de byte
            pos[i] = pos[i - 1] + count[i - 1];
            count[i - 1] = 0;
        }
        for (int i = 0; i < nElems; ++i) { // Percorre elementos copiados
            // Reposiciona elementos com base na ordenação do byte atual
            short dig = (retonaChave(cop[i]) >> shift*8) & 255;
            elemns[pos[dig]] = cop[i];
            pos[dig]++;
        }
    }
    free(cop); // Libera vetor auxiliar
}

// Cast bytes para tipo correto
RetornaChave_f castChave(void *chave, size_t tamanhoChave) {
    RetornaChave_f retorno = 0; // Inicializa retorno
    memcpy(&retorno, chave, tamanhoChave); // Copia bytes com o tamanho correto
    return retorno;
}
