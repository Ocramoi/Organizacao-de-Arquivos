# include <stdio.h>
# include "convertePrefixo.h"

/* Esse programa checa que a função convertePrefixo está funcinando corretamente */
int main(int argc, char* argv[]) {

    /* Checa pelos argumentos */
    if(argc != 2) {
        printf("Uso: ./<nome do programa> <arquivo .csv para testar>\n");
        return 1;
    }
    
    /* Tenta abrir o .csv */
    FILE* file = fopen(argv[1], "r");
    if(file == NULL) {
        printf("Não foi possível abrir %s!\n", argv[1]);
        return 1;
    }

    /* Buffer para guardar os hashes dos prefixos */
    int hash_buffer[8192];

    /* Buffer para leitura */
    char line_buffer[256]; 

    /* Lê a linha de cabeçalho para o buffer (ela será descartada) */
    fscanf(file, "%255[^\n] ", line_buffer);

    /* 
        Lê o csv e gera os hashes, para imediatamente se for estourar 
        o buffer que guarda os hashes
    */
    int hashes_read = 0;
    while(hashes_read < 8192 && fscanf(file, "%255[^\n] ", line_buffer) != EOF) {
        
        /* 
            Pega o hash da linha atual (a função hash só lê os primeiros
            5 chars da string então ela lerá o campo "Prefixo do veiculo"
            corretamente
        */
        int cur_hash = convertePrefixo(line_buffer);

        /* Printa a string e seu hash */
        if(cur_hash != -1)
            printf("%.5s = 0x%08X\n", line_buffer, cur_hash);
        else
            printf("%.5s = inválido\n", line_buffer);

        /* Guarda o hash na lista */
        hash_buffer[hashes_read] = cur_hash;
        hashes_read++;

    }

    /* Checa se a lista teve hashes repetidos diferentes de -1 */
    int repeated_hashes = 0;
    for(int i = 0; i < hashes_read; i++) {
        for(int j = 0; j < i; j++) {
            if(hash_buffer[i] == hash_buffer[j] && hash_buffer[i] != -1) {
                printf("ERRO: hashes repetidos nas linhas %d e %d!\n", j + 1, i + 1);
                repeated_hashes++;
            }
        }
    }

    /* Printa se há hashes repetidos */
    if(repeated_hashes == 0)
        printf("SUCESSO: Não há hashes válidos repetidos!\n");
    else
        printf("ERRO: Há %d hashes repetidos!\n", repeated_hashes);
    return 0;

}