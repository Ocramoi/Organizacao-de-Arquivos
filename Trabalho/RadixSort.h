#ifndef __RADIXSORT_H_
#define __RADIXSORT_H_

#include <stdlib.h>

typedef unsigned long long RetornaChave_f; // Tipo para a função de retorno da chave para tipo genérico (casting para unsigned long long para adequar tipos de até 8 bytes)

/* Casting de bytes para tipo correto */
RetornaChave_f castChave(void *chave, size_t tamanhoChave);

/* Radix sort implementado com count sort na base 256 com tipos genéricos de unsigned de até 8 bytes */
void radixSort(void **elemns, size_t tamElem, int nElems, RetornaChave_f (*retonaChave)(void*));

#endif // __RADIXSORT_H_
