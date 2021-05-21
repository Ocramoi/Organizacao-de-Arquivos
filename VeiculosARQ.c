#include "VeiculosARQ.h"
#include "LeLinha.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona veículo a partir de [tempRegistro] ao arquivo [tabela] já criado */
int adicionaVeiculoARQ(FILE *tabela, char *registro, int64_t *offset) {
    // Erro de ponteiros
    if (!tabela || !registro || !offset)
        return -1;

    char *tempLeitura = strtok(registro, ","); // Inicializa tokenização da string de registro

    // Contabiliza e escreve se registro foi removido
    int controleRemovido;
    if (tempLeitura[0] == '*'){
        controleRemovido = 1;
        memmove(tempLeitura, tempLeitura+1, strlen(tempLeitura));
    }
    else
        controleRemovido = 0;
    fwrite(controleRemovido ? "0" : "1", sizeof(char), 1, tabela);

    // Captura e armazena os dados do arquivo CSV, fez-se o armazenamento para contabilizar o tamanho do arquivo
    char *prefixo = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *data = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *quantidadeLugares  = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *codLinha  = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *modelo = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    char *categoria = strdup(tempLeitura);
    tempLeitura = strtok(NULL, ",");
    
    // Define-se o tamanho do campo Modelo
    int32_t controleModelo;
    if (!strcmp(modelo, "NULO"))
        controleModelo = 0;
    else
        controleModelo = strlen(modelo);
    
    // Define-se o tamanho do campo Categoria
    int32_t controleCat;
    if (!strcmp(categoria, "NULO"))
        controleCat = 0;
    else
        controleCat = strlen(categoria);
    
    // Definição e escrita do tamanho de registro 4*int+15*char = 31 + variável
    int32_t tamanhoRegistro;
    tamanhoRegistro = 31 + controleModelo + controleCat;
    fwrite(&tamanhoRegistro, sizeof(int32_t), 1, tabela);
    
    // Escrita do campo Prefixo.
    fwrite(prefixo, sizeof(char), 5, tabela); 

    // Escreve strings tratando NULO quando necessário e registrando informações numéricas
    if (!strcmp(data, "NULO"))
        fwrite("\0@@@@@@@@@", sizeof(char), 10, tabela);
    else
        fwrite(data, sizeof(char), 10, tabela);

    // Verificação da quantidade de lugares
    int32_t controleLugares;
    if (!strcmp(quantidadeLugares, "NULO"))
        controleLugares = -1;
    else
        controleLugares = atoi(quantidadeLugares);
    fwrite(&controleLugares, sizeof(int32_t), 1, tabela);

    // Verificação do codLinha
    int32_t controleLinha;
    if (!strcmp(codLinha, "NULO"))
        controleLinha = -1;
    else
        controleLinha = atoi(codLinha);
    
    // Escrita dos demais campos.
    fwrite(&controleLinha, sizeof(int32_t), 1, tabela);
    fwrite(&controleModelo, sizeof(int32_t), 1, tabela);
    fwrite(modelo, sizeof(char), controleModelo, tabela);
    fwrite(&controleCat, sizeof(int32_t), 1, tabela);
    fwrite(categoria, sizeof(char), controleCat, tabela);
    tempLeitura = strtok(NULL, ",");

    // Contabiliza tamanho do registro
    *offset += 36 + controleModelo + controleCat;

    // Retorna registro adicionado
    return controleRemovido;
}

/* "CREATE TABLE" de veículos a partir do csv de nome [entrada], salvando como o arquivo binário [saida] (sem memória auxiliar) */
int criaTabelaVeiculosARQ(char *entrada, char *saida) {
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
    int64_t proxInit = 174;
    int32_t nRegInit = 0;
    fwrite("0", sizeof(char), 1, arqSaida);
    fwrite(&proxInit, sizeof(int64_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(&nRegInit, sizeof(int32_t), 1, arqSaida);
    fwrite(registro, sizeof(char), 18, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 35, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 42, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 26, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 17, arqSaida); registro = strtok(NULL, ",");
    fwrite(registro, sizeof(char), 20, arqSaida);

    // Escreve registros do csv linha a linha
    int32_t nReg = 0, nRemovidos = 0;
    int64_t offset = 175;
    while (tempRegistro) {
        free(tempRegistro);
        tempRegistro = leLinha(arqEntrada);
        if (!tempRegistro) break;

        int r = adicionaVeiculoARQ(arqSaida, tempRegistro, &offset);
        if (r == -1) {
            printf("::: 114\n");
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

int buscaVeiculos(char *entrada, char *nomeDoCampo, char *valor) {
    // Abre arquivos a partir do nome
    FILE *arqEntrada = fopen(entrada, "rb");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        return 1;
    }

    //Leitura do Registro de Cabeçalho
    size_t len;
    char descrevePrefixo[19], descreveData[36], descreveLugares[43], descreveLinha[27], descreveModelo[18], descreveCategoria[21];
    fseek(arqEntrada, 17, SEEK_SET);
    len = fread(descrevePrefixo, sizeof(char), 18, arqEntrada);
    descrevePrefixo[len] = '\0';
    len = fread(descreveData, sizeof(char), 35, arqEntrada);
    descreveData[len] = '\0';
    len = fread(descreveLugares, sizeof(char), 42, arqEntrada);
    descreveLugares[len] = '\0';
    len = fread(descreveLinha, sizeof(char), 26, arqEntrada);
    descreveLinha[len] = '\0';
    len = fread(descreveModelo, sizeof(char), 17, arqEntrada);
    descreveModelo[len] = '\0';
    len = fread(descreveCategoria, sizeof(char), 20, arqEntrada);
    descreveCategoria[len] = '\0';
    
    //Leitura dos Registro de Dados
    char removido[2], prefixo[6], data[11], dia[3], mes[3], ano[5], descricaoMes[20];
    char *modelo;
    modelo = malloc (100 * sizeof (char));
    char *categoria;
    categoria = malloc (100 * sizeof (char));
    int32_t tamanhoRegistro, quantidaDeLugares, codLinha, tamanhoModelo, tamanhoCategoria;
    long int posicao;
    posicao = 175; // Byteoffset do primeiro registro de dados
    fseek(arqEntrada, posicao, SEEK_SET);
    int pertence; 

    while (!feof(arqEntrada))	
 	{
        pertence = 0;
        len = fread(removido, sizeof(char), 1, arqEntrada);
        removido[len] = '\0';
        if (removido[0] == '1') {
            // Leitura do tamanho do Registro
            fread(&tamanhoRegistro, sizeof(int32_t), 1, arqEntrada);
            
            // Leitura do prefixo
            len = fread(prefixo, sizeof(char), 5, arqEntrada);
            prefixo[len] = '\0';
            if (strcmp(nomeDoCampo, "prefixo") == 0){
                if (strcmp(valor, prefixo) == 0){
                    pertence = 1;
                }
            }       
            // Leitura da data
            len = fread(data, sizeof(char), 10, arqEntrada);
            data[len] = '\0';

            // Leitura da quantidade de lugares
            fread(&quantidaDeLugares, sizeof(int32_t), 1, arqEntrada);

            // Leitura do codigo de linha  
            fread(&codLinha, sizeof(int32_t), 1, arqEntrada);

            // Leitura do tamanho do modelo
            fread(&tamanhoModelo, sizeof(int32_t), 1, arqEntrada);
            if(tamanhoModelo > 0){
                //modelo = realloc (modelo, tamanhoModelo*sizeof(char));
                len = fread(modelo, sizeof(char), tamanhoModelo, arqEntrada);
                modelo[len] = '\0';
            }

            // Leitura do tamanho da categoria 
            fread(&tamanhoCategoria, sizeof(int32_t), 1, arqEntrada);
            if(tamanhoCategoria > 0){
                //categoria = realloc (categoria, (tamanhoCategoria*sizeof(char));
                len = fread(categoria, sizeof(char), tamanhoCategoria, arqEntrada);
                categoria[len] = '\0';
            }

            //Impressão
            if (pertence == 1){
                // Impressão do Prefixo
                printf("%s: %s\n", descrevePrefixo, prefixo);
                
                // Impressão da data
                if (data[1] == '@'){
                    printf("%s: campo com valor nulo\n", descreveData);
                }
                else {
                    dia[0] = data[8]; dia[1] = data[9]; dia[2] = '\0';
                    mes[0] = data[5]; mes[1] = data[6]; mes[2] = '\0';
                    ano[0] = data[0]; ano[1] = data[1]; ano[2] = data[2]; ano[3] = data[3]; ano[4] = '\0';
                    if (strcmp(mes, "01") == 0){
                        strcpy(descricaoMes, "janeiro");
                    } else if (strcmp(mes, "02") == 0){
                        strcpy(descricaoMes, "fevereiro");
                    } else if (strcmp(mes, "03") == 0){
                        strcpy(descricaoMes, "março"); 
                    } else if (strcmp(mes, "04") == 0){
                        strcpy(descricaoMes, "abril"); 
                    } else if (strcmp(mes, "05") == 0){
                        strcpy(descricaoMes, "maio"); 
                    } else if (strcmp(mes, "06") == 0){
                        strcpy(descricaoMes, "junho"); 
                    } else if (strcmp(mes, "07") == 0){
                        strcpy(descricaoMes, "julho"); 
                    } else if (strcmp(mes, "08") == 0){
                        strcpy(descricaoMes, "agosto"); 
                    } else if (strcmp(mes, "09") == 0){
                        strcpy(descricaoMes, "setembro"); 
                    } else if (strcmp(mes, "10") == 0){
                        strcpy(descricaoMes, "outubro");
                    } else if (strcmp(mes, "11") == 0){
                        strcpy(descricaoMes, "novembro"); 
                    } else {
                        strcpy(descricaoMes, "dezembro"); 
                    }
                    printf("%s: %s de %s de %s\n", descreveData, dia, descricaoMes, ano);
                }

                // Impressão da quantidade de lugares
                if(quantidaDeLugares == -1){
                    printf("%s: campo com valor nulo\n", descreveLugares);
                }
                else{
                    printf("%s: %d\n", descreveLugares, quantidaDeLugares);
                }

                // Impressão do codigo de linha  
                if(codLinha == -1){
                    printf("%s: campo com valor nulo\n", descreveLinha);
                }
                else{
                    printf("%s: %d\n", descreveLinha, codLinha);
                }

                // Impressão do modelo
                if(tamanhoModelo == 0){
                    printf("%s: campo com valor nulo\n", descreveModelo);
                }
                else{
                    printf("%s: %s\n", descreveModelo, modelo);
                }

                // Impressão do tamanho da categoria 
                if(tamanhoCategoria == 0){
                    printf("%s: campo com valor nulo\n", descreveCategoria);
                }
                else{
                    printf("%s: %s\n", descreveCategoria, categoria);
                }
                printf("\n");
            }
        }//Ignora os removidos
        else if (removido[0] == '0'){
            fread(&tamanhoRegistro, sizeof(int32_t), 1, arqEntrada); 
            fseek(arqEntrada, tamanhoRegistro, SEEK_CUR); // Desloca para o próximo registro
        }
        else{
            break;
        }
  	}

    // Fecha arquivo de entrada
    fclose(arqEntrada);

    return 0;
}
