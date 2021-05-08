#ifndef __LINHAS_H_
#define __LINHAS_H_

#include <stdlib.h>

// Estrutura dado do registro de cada linha
typedef struct {
    int32_t tamanhoRegistro,
        codLinha,
        tamanhoNome,
        tamanhoCor;
    char removido,
        aceitaCartao,
        *nomeLinha,
        *corLinha;
} LINHA_t;

// Estrutura de dados da tabela de veículos
typedef struct {

} LINHAS_t;

#endif // __LINHAS_H_
