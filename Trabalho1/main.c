#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura dado do registro de cada veículo
typedef struct {
    char removido,
        prefixo[5],
        data[10];
    int32_t tamanhoRegistro,
        tamanhoModelo,
        tamanhoCategoria,
        quantidadeLugares,
        codLinha;
    char *modelo,
        *categoria;
} VEICULO_t;

// Estrutura de dados da tabela de veículos
typedef struct {
    char status;
    int64_t byteProxReg;
    int32_t nroRegistros,
        nroRegRemovidos;
    char descrevePrefixo[18],
        descreveData[35],
        descreveLugares[42],
        descreveLinha[26],
        descreveModelo[17],
        desceveCategoria[20];
    VEICULO_t *veiculos;
} VEICULOS_t;

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

#define STR_BUFFER 64 // buffer do realloc de string

char *leLinha(FILE* entrada) { // função que lê string com tratamento
    char *retorno = NULL; // inicializa o ponteiro da frase lida
    int cont = 0; // inicializa a varável que auxiliará o tratamento de tamanho

    do {
        if (cont % STR_BUFFER == 0) { // conferência do tamanho como próximo múltiplo do buffer
            retorno = (char* ) realloc(retorno, ((cont/STR_BUFFER + 1) * STR_BUFFER) * sizeof(char)); // aumenta o tamanho da string em função do buffer caso tenha ultrapassado um novo múltiplo, diminuindo o gasto computacional com realocação
        }
        retorno[cont] = fgetc(entrada); // lê novo caracter
        cont++; // atualiza o tamanho da string
    } while (retorno[cont-1] != '\n' && retorno[cont-1] != EOF); // repete até o final da leitura de string

    if (cont >= 2 && retorno[cont - 2] == '\r') { // confere pelo caracter de formatação '\r'
        retorno[cont - 2] = '\0'; // substitui o caracter por fim de string
        retorno = (char* ) realloc(retorno, (cont-1) * sizeof(char)); // diminui o tamanho da string em 1 caracter
    }
    else {
        retorno[cont-1] = '\0'; // formata a string com o caracter de final de string
        retorno = (char* ) realloc(retorno, cont * sizeof(char)); // realoca o ponteiro da string para seu tamanho desconsiderando o buffer
    }

    // Trata retorno
    if(strlen(retorno) == 0) {
        free(retorno);
        return NULL;
    }
    return retorno;
}

int main(int argc, char **argv) {
    // Lê opção e dados necessários, chamando então função desejada
    int opcao;
    char *entrada = leLinha(stdin);
    sscanf(entrada, "%d", &opcao);
    switch (opcao) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            printf("Opção digitada inválida!\n");
            break;
    }

    // Libera memória dinamicamente alocada
    free(entrada);
    return 0;
}
