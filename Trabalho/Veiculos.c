#include "Veiculos.h"
#include "LeLinha.h"
#include "TratamentoDeValores.h"
#include "convertePrefixo.h"
#include "ArvoreB.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Adiciona veículo a partir de [tempRegistro] ao arquivo [tabela] já criado */
int adicionaVeiculoARQ(FILE *tabela, char *registro, int64_t *offset) {
    // Trata erro de ponteiros
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
    
    // Define-se os tamanho do campo Modelo e Categoria
    int32_t controleModelo;
    if (!strcmp(modelo, "NULO"))
        controleModelo = 0;
    else
        controleModelo = strlen(modelo);
    
    int32_t controleCat;
    if (!strcmp(categoria, "NULO"))
        controleCat = 0;
    else
        controleCat = strlen(categoria);
    
    // Definição e escrita do tamanho de registro [4*int + 15*char] = 31 + variável
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
    if (!arqEntrada) // Confere falha na abertura dos arquivos
        return 1;

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

/* Libera memória de estrutura de veículo lida */
int destroiVeiculo (VEICULO_t *veiculo) {
    // Confere registro
    if (!veiculo)
        return 1;

    // Libera memória associada
    free(veiculo->categoria); free(veiculo->data); free(veiculo->modelo);
    free(veiculo);
    return 0;
}

/* Cria e popula estrutura de veículo a partir do ponteiro de arquivo dado */
VEICULO_t *leVeiculo(FILE *arq) {
    // Confere arquivo
    if (!arq)
        return NULL;

    // Estrutura de veículo
    VEICULO_t *veiculo = malloc(sizeof(VEICULO_t));

    // Lê informações iniciais e trata se removido
    char removido; fread(&removido, sizeof(char), 1, arq);
    int32_t offset; fread(&offset, sizeof(int32_t), 1, arq);
    if (removido == '0') { // Confere se removido, o contabilizando e pulando para o próximo registro
        fseek(arq, offset, SEEK_CUR);
        free(veiculo);
        return NULL;
    }

    // Lê valores brutos do registro e os formata
    fread(veiculo->prefixo, sizeof(char), 5, arq); veiculo->prefixo[5] = '\0';
    char curData[11]; fread(curData, sizeof(char), 10, arq); curData[10] = '\0';
    veiculo->data = trataData(curData);
    fread(&(veiculo->quantidadeLugares), sizeof(int32_t), 1, arq);
    fread(&(veiculo->codLinha), sizeof(int32_t), 1, arq);

    int tamModelo; fread(&tamModelo, sizeof(int32_t), 1, arq);
    if (tamModelo) {
        veiculo->modelo = calloc(tamModelo + 1, sizeof(char));
        fread(veiculo->modelo, sizeof(char), tamModelo, arq);
    } else
        veiculo->modelo = NULL;

    int tamCateg; fread(&tamCateg, sizeof(int32_t), 1, arq);
    if (tamCateg){
        veiculo->categoria = calloc(tamCateg + 1, sizeof(char));
        fread(veiculo->categoria, sizeof(char), tamCateg, arq);
    } else
        veiculo->categoria = NULL;

    // Retorna objeto criado
    return veiculo;
}

/* Seleciona e exibe todos os registros do arquivo binário [tabela] de veículos */
int selectAllVeiculos(char *tabela) {
    return selectVeiculos(tabela, NULL, NULL);
}

/* "SELECT * from Veiculos WHERE ..." -> Seleciona e exibe todos os registros do arquivo binário de nome [tabela] de veículos com [campo] de [valor] */
int selectVeiculos(char *tabela, char *campo, char *valor) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return 1;

    // Se valor de campo a ser pesquisado dado, requer valor de pesquisa
    if (campo && !valor)
        return 1;

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arq);
    if (status != '1') {
        fclose(arq);
        return 1;
    }
    fseek(arq, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arq);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arq);
    if (nroRegs == 0) {
        fclose(arq);
        return -1;
    }

    // Lê strings de cabeçalho
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

    // Cria valor de pesquisa a partir da string dada
    char *strTratado = NULL;
    if (valor)
        strTratado = strtok(valor, "\"");
    int numTratado;
    if (strTratado)
        numTratado = atoi(strTratado);

    // Lê registro a registro contabilizando removidos
    int32_t controleRemovidos = 0;
    for (int i = 0; i < nroRegs + nroRemvs; ++i) {
        // Lê novo objeto
        VEICULO_t *atual = leVeiculo(arq);

        // Contabiliza se registro removido
        if (arq && !atual) {
            controleRemovidos++;
            continue;
        }

        // Ignora registros não compatíveis caso feita pesquisa por campo
        if (campo) {
            if (!strcmp(campo, "prefixo") && strcmp(atual->prefixo, strTratado)) {
                destroiVeiculo(atual);
                continue;
            }
            if (!strcmp(campo, "data") && strcmp(atual->data, strTratado)) {
                destroiVeiculo(atual);
                continue;
            }
            if (!strcmp(campo, "quantidadeLugares") && numTratado != atual->quantidadeLugares) {
                destroiVeiculo(atual);
                continue;
            }
            if (!strcmp(campo, "modelo") && strcmp(atual->modelo, strTratado)) {
                destroiVeiculo(atual);
                continue;
            }
            if (!strcmp(campo, "categoria") && strcmp(atual->categoria, strTratado)) {
                destroiVeiculo(atual);
                continue;
            }
        }

        // Exibe informações no formato indicado
        printf("%s: %s\n", prefixo, atual->prefixo);

        printf("%s: ", modelo);
        if (atual->modelo)
            printf("%s\n", atual->modelo);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", categoria);
        if (atual->categoria)
            printf("%s\n", atual->categoria);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", data);
        if (atual->data)
            printf("%s\n", atual->data);
        else
            printf("campo com valor nulo\n");

        printf("%s: ", lugares);
        if (atual->quantidadeLugares != -1)
            printf("%d\n", atual->quantidadeLugares);
        else
            printf("campo com valor nulo\n");
        printf("\n");

        // Libera registro criado anteriormente
        destroiVeiculo(atual);
    }

    // Libera memória alocada
    free(prefixo); free(data); free(lugares); free(linha); free(modelo); free(categoria);
    // Fecha arquivo da tabela
    fclose(arq);

    // Confere se número de registros removidos correto
    if (controleRemovidos != nroRemvs)
        return 1;

    return 0;
}

VEICULO_t *regParaObj(char *registro) {
    return NULL;
}

/* "INSERT INTO Veiculos ..." -> Insere informações lidas em [registro] na tabela do arquivo [nomeArq] dada */
int insertVeiculo(char *nomeArq, char *registro) {
    // Confere ponteiros passados
    FILE *tabela = fopen(nomeArq, "r+b");
    if (!tabela || !registro || !tabela)
        return 1;

    // Checa o status do arquivo [nomeArq] para o [registro] ser escrito 
    char status; fread(&status, sizeof(char), 1, tabela);
    if (status != '1') {
        fclose(tabela);
        return 1;
    }

    // Altera o status do arquivo para em uso [0] 
    fseek(tabela, 0, SEEK_SET);
    fwrite("0", sizeof(char), 1, tabela);
    // Busca os campos armazenados no cabeçalho do arquivo [nomeArq] byte offset [offset] e numeros de registros [numReg]
    int64_t offset; fread(&offset, sizeof(int64_t), 1, tabela);
    int32_t numReg; fread(&numReg, sizeof(int32_t), 1, tabela);

    // Posiciona o arquivo [nomeArq] no byte offset/final do arquivo
    fseek(tabela, 0, SEEK_END);

    // Variáveis de armazenamento dos dados de [registro]
    char tempPrefixo[10],
        tempData[25],
        tempModelo[100],
        tempCategoria[100],
        tempQuantLugar[50],
        tempCodLinha[50],
        resto[150];
   
    // Leitura dos dados de [registro] e alocação
    sscanf(registro, "\"%[^\"]\" %s %s %s %[^\n]", tempPrefixo, tempData, tempQuantLugar, tempCodLinha, resto);
    if (resto[0] == '"')
        sscanf(resto, "\"%[^\"]\" %s", tempModelo, tempCategoria);
    else
        sscanf(resto, "%s %s", tempModelo, tempCategoria);
    
    // Trata os dados de [registro] inseridos
    char *prefixo = trataAspas(tempPrefixo, 10),
        *data = trataAspas(tempData, 25),
        *modelo = trataAspas(tempModelo, 100),
        *categoria = trataAspas(tempCategoria, 50);
    int32_t quantLugares, codLinha;

    if (!strcmp(data, "NULO")) {
        data = realloc(data, 1);
        data[0] = '\0';
    }

    if (!strcmp(modelo, "NULO")) {
        modelo = realloc(modelo, 1);
        modelo[0] = '\0';
    }

    if (!strcmp(categoria, "NULO")) {
        categoria = realloc(categoria, 1);
        categoria[0] = '\0';
    }

    if (!strcmp(tempQuantLugar, "NULO"))
        quantLugares = -1;
    else
        quantLugares = atoi(tempQuantLugar);

    if (!strcmp(tempCodLinha, "NULO"))
        codLinha = -1;
    else
        codLinha = atoi(tempCodLinha);

    // Escrita dos dados de [registro] no arquivo [nomeArq] já posicionado no final [offset]
    fwrite("1", sizeof(char), 1, tabela);
    int32_t tamModelo = strlen(modelo),
        tamCategoria = strlen(categoria),
        tam = 31 + tamModelo + tamCategoria;
    fwrite(&tam, sizeof(int32_t), 1, tabela);
    fwrite(prefixo, sizeof(char), 5, tabela);
    if (strlen(data))
        fwrite(data, sizeof(char), 10, tabela);
    else
        fwrite("\0@@@@@@@@@", sizeof(char), 10, tabela);
    fwrite(&quantLugares, sizeof(int32_t), 1, tabela);
    fwrite(&codLinha, sizeof(int32_t), 1, tabela);
    fwrite(&tamModelo, sizeof(int32_t), 1, tabela);
    fwrite(modelo, sizeof(char), tamModelo, tabela);
    fwrite(&tamCategoria, sizeof(int32_t), 1, tabela);
    fwrite(categoria, sizeof(char), tamCategoria, tabela);

    // Atualização do número de registros [numReg], byte offset [offset]
    int64_t proxReg = offset + tam + 5;
    numReg++;
    fseek(tabela, 1, SEEK_SET);
    fwrite(&proxReg, sizeof(int64_t), 1, tabela);
    fwrite(&numReg, sizeof(int32_t), 1, tabela);

    // Atualização do status do arquivo [nomeArq]
    fseek(tabela, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, tabela);

    // Libera memória alocada
    free(prefixo); free(data); free(modelo); free(categoria);

    // Fecha o arquivo
    fclose(tabela);
    return 0;
}

int pesquisaVeiculoArvB(char *arqTabela, char *arqArvore, char *prefixo) {
    if (!arqTabela || !arqArvore || !prefixo)
        return -1;

    char *valorTratado = trataAspas(prefixo, 100);
    ARVB_t *arvore = populaArvB(arqArvore);
    if (!arvore)
        return 1;

    int64_t offSetPesquisa = pesquisaArvB(arvore, convertePrefixo(valorTratado));
    if (offSetPesquisa > 0)
        exibeVeiculoOffset(arqTabela, offSetPesquisa);

    free(arvore); free(valorTratado);

    if (offSetPesquisa > 0)
        return 0;
    return 2;
}

int exibeVeiculoOffset(char *tabela, int64_t offset) {
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return -1;

    fseek(arq, offset, SEEK_SET);
    VEICULO_t *tempVeiculo = leVeiculo(arq);
    fseek(arq, 0, SEEK_SET);

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arq);
    if (status != '1') {
        fclose(arq);
        return 1;
    }
    fseek(arq, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arq);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arq);
    if (nroRegs == 0) {
        fclose(arq);
        return -1;
    }

    // Lê strings de cabeçalho
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

    // Exibe informações no formato indicado
    printf("%s: %s\n", prefixo, tempVeiculo->prefixo);

    printf("%s: ", modelo);
    if (tempVeiculo->modelo)
        printf("%s\n", tempVeiculo->modelo);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", categoria);
    if (tempVeiculo->categoria)
        printf("%s\n", tempVeiculo->categoria);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", data);
    if (tempVeiculo->data)
        printf("%s\n", tempVeiculo->data);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", lugares);
    if (tempVeiculo->quantidadeLugares != -1)
        printf("%d\n", tempVeiculo->quantidadeLugares);
    else
        printf("campo com valor nulo\n");
    printf("\n");

    destroiVeiculo(tempVeiculo);
    // Libera memória alocada
    free(prefixo); free(data); free(lugares); free(linha); free(modelo); free(categoria);
    fclose(arq);
    return 0;
}

/* "CREATE INDEX ... Veiculos" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreVeiculos(char *tabela, char *arvore) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arqTabela = fopen(tabela, "rb");
    if (!tabela || !arqTabela)
        return 1;

    // Lê e confere status do arquivo
    char status; fread(&status, sizeof(char), 1, arqTabela);
    if (status != '1') {
        fclose(arqTabela);
        return 1;
    }
    fseek(arqTabela, sizeof(int64_t), SEEK_CUR);

    // Lê contagem de registros
    int32_t nroRegs; fread(&nroRegs, sizeof(int32_t), 1, arqTabela);
    int32_t nroRemvs; fread(&nroRemvs, sizeof(int32_t), 1, arqTabela);
    if (nroRegs == 0) {
        fclose(arqTabela);
        return -1;
    }
    fseek(arqTabela, 174, SEEK_SET);

    ARVB_t *arvoreVeiculos = populaArvB(arvore);
    for (int i = 0; i < nroRegs + nroRemvs; ++i) {
        int64_t offsetAtual = ftell(arqTabela);
        VEICULO_t *veiculoAtual = leVeiculo(arqTabela);
        if (!veiculoAtual) {
            fclose(arqTabela);
            return 1;
        }
        if (!veiculoAtual->removido) {
            destroiVeiculo(veiculoAtual);
            continue;
        }
        adicionaRegistroArvB(arvoreVeiculos, convertePrefixo(veiculoAtual->prefixo), offsetAtual);
    }
    free(arvoreVeiculos);
    fclose(arqTabela);
    return 0;
}

int adicionaRegistroArvore(char *arvore, char *registro) {}
