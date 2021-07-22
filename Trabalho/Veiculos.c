#include "Veiculos.h"
#include "LeLinha.h"
#include "TratamentoDeValores.h"
#include "convertePrefixo.h"
#include "ArvoreB.h"
#include "RadixSort.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exibe campos de [veiculo] com descrição do [cabecalho] */
int exibeDescreveVeiculo(CABECALHO_VEICULOS_t *cabecalho, VEICULO_t *veiculo) {
    // Confere argumentos
    if (!cabecalho || !veiculo)
        return 1;

    // Exibe informações no formato indicado
    printf("%s: %s\n", cabecalho->prefixo, veiculo->prefixo);

    printf("%s: ", cabecalho->modelo);
    if (veiculo->modelo)
        printf("%s\n", veiculo->modelo);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", cabecalho->categoria);
    if (veiculo->categoria)
        printf("%s\n", veiculo->categoria);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", cabecalho->data);
    if (veiculo->data)
        printf("%s\n", veiculo->data);
    else
        printf("campo com valor nulo\n");

    printf("%s: ", cabecalho->lugares);
    if (veiculo->quantidadeLugares != -1)
        printf("%d\n", veiculo->quantidadeLugares);
    else
        printf("campo com valor nulo\n");

    return 0;
}

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
    fread(&(veiculo->removido), sizeof(char), 1, arq);
    fread(&(veiculo->tamRegistro), sizeof(int32_t), 1, arq);
    if (veiculo->removido == '0') { // Confere se removido, o contabilizando e pulando para o próximo registro
        fseek(arq, veiculo->tamRegistro, SEEK_CUR);
        free(veiculo);
        return NULL;
    }

    // Lê valores brutos do registro e os formata
    fread(veiculo->prefixo, sizeof(char), 5, arq); veiculo->prefixo[5] = '\0';
    fread(veiculo->dataOriginal, sizeof(char), 10, arq); veiculo->dataOriginal[10] = '\0';
    veiculo->data = trataData(veiculo->dataOriginal);
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

    // Lê cabeçalho do arquivo
    CABECALHO_VEICULOS_t *cabecalho = leCabecalhoVeiculos(arq);

    // Confere status do arquivo
    if (cabecalho->status != '1') {
        destroiCabecalhoVeiculos(cabecalho);
        fclose(arq);
        return 1;
    }

    // Confere número de registros
    if (cabecalho->nroRegs == 0) {
        destroiCabecalhoVeiculos(cabecalho);
        fclose(arq);
        return -1;
    }

    // Cria valor de pesquisa a partir da string dada
    char *strTratado = NULL;
    if (valor)
        strTratado = strtok(valor, "\"");
    int numTratado;
    if (strTratado)
        numTratado = atoi(strTratado);

    // Lê registro a registro contabilizando removidos
    int32_t controleRemovidos = 0;
    for (int i = 0; i < cabecalho->nroRegs + cabecalho->nroRems; ++i) {
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

        // Exibe veículo compatível
        exibeDescreveVeiculo(cabecalho, atual);

        // Libera registro criado anteriormente
        destroiVeiculo(atual);
    }

    // Fecha arquivo da tabela
    fclose(arq);

    // Confere se número de registros removidos correto
    if (controleRemovidos != cabecalho->nroRems)
        return 1;

    // Libera memória alocada
    destroiCabecalhoVeiculos(cabecalho);

    return 0;
}

/* Converte linha de [resgitro] para objeto de VEICULO_t */
VEICULO_t *regParaVeiculo(char *registro) {
    // Cria veículo para retorno
    VEICULO_t *veiculo = malloc(sizeof(VEICULO_t));
    if (!veiculo)
        return NULL;

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

    // Retorno registro lido
    veiculo->removido = '1';
    memcpy(veiculo->prefixo, prefixo, 6);
    veiculo->data = data;
    veiculo->quantidadeLugares = quantLugares;
    veiculo->codLinha = codLinha;
    veiculo->modelo = modelo;
    veiculo->categoria = categoria;
    free(prefixo);

    return veiculo;
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

    // Lê cabeçalho do arquivo [nomeArq]
    fseek(tabela, 0, SEEK_SET);
    CABECALHO_VEICULOS_t *cabecalho = leCabecalhoVeiculos(tabela);

    VEICULO_t *tempVeiculo = regParaVeiculo(registro);

    // Posiciona o arquivo [nomeArq] no byte offset final do arquivo
    fseek(tabela, 0, SEEK_END);

    // Escrita dos dados de [registro] no arquivo [nomeArq] já posicionado no final [offset]
    fwrite(&(tempVeiculo->removido), sizeof(char), 1, tabela);
    int32_t tamModelo = strlen(tempVeiculo->modelo),
        tamCategoria = strlen(tempVeiculo->categoria),
        tam = 31 + tamModelo + tamCategoria;
    fwrite(&tam, sizeof(int32_t), 1, tabela);
    fwrite(tempVeiculo->prefixo, sizeof(char), 5, tabela);
    if (strlen(tempVeiculo->data))
        fwrite(tempVeiculo->data, sizeof(char), 10, tabela);
    else
        fwrite("\0@@@@@@@@@", sizeof(char), 10, tabela);
    fwrite(&(tempVeiculo->quantidadeLugares), sizeof(int32_t), 1, tabela);
    fwrite(&(tempVeiculo->codLinha), sizeof(int32_t), 1, tabela);
    fwrite(&tamModelo, sizeof(int32_t), 1, tabela);
    fwrite(tempVeiculo->modelo, sizeof(char), tamModelo, tabela);
    fwrite(&tamCategoria, sizeof(int32_t), 1, tabela);
    fwrite(tempVeiculo->categoria, sizeof(char), tamCategoria, tabela);

    // Atualização do número de registros [numReg], byte offset [offset] e status
    int64_t proxReg = cabecalho->byteOffset + tam + 5;
    cabecalho->nroRegs++;
    fseek(tabela, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, tabela);
    fwrite(&proxReg, sizeof(int64_t), 1, tabela);
    fwrite(&(cabecalho->nroRegs), sizeof(int32_t), 1, tabela);

    // Libera memória alocada
    destroiVeiculo(tempVeiculo);
    destroiCabecalhoVeiculos(cabecalho);

    // Fecha o arquivo
    fclose(tabela);
    return 0;
}

// Lê e exibe veículo lido na [tabela] no [offset]
int exibeVeiculoOffset(char *tabela, int64_t offset) {
    // Abre e confere arquivo
    FILE *arq = fopen(tabela, "rb");
    if (!tabela || !arq)
        return -1;

    // Vai ao offset e lê veículo
    fseek(arq, offset, SEEK_SET);
    VEICULO_t *tempVeiculo = leVeiculo(arq);

    // Lê cabeçalho de arquivo
    fseek(arq, 0, SEEK_SET);
    CABECALHO_VEICULOS_t *cabecalho = leCabecalhoVeiculos(arq);

    // Confere status do arquivo
    if (cabecalho->status != '1') {
        fclose(arq);
        destroiCabecalhoVeiculos(cabecalho);
        return 1;
    }

    // Confere número de registros
    if (cabecalho->nroRegs == 0) {
        fclose(arq);
        destroiCabecalhoVeiculos(cabecalho);
        return -1;
    }

    // Exibe veículo com descrição do cabecalho
    exibeDescreveVeiculo(cabecalho, tempVeiculo);

    destroiVeiculo(tempVeiculo);

    // Libera memória alocada
    destroiCabecalhoVeiculos(cabecalho);
    fclose(arq);
    return 0;
}

/* "SELECT * from Veiculos WHERE ..." -> Seleciona e exibe registro do arquivo binário [arqTabela] de veículos com [prefixo] dado com pesquisa na árvore [arqArvore] */
int pesquisaVeiculoArvB(char *arqTabela, char *arqArvore, char *prefixo) {
    // Confere parâmetros
    if (!arqTabela || !arqArvore || !prefixo)
        return -1;

    // Popula estrutura da árvore para pesquisa
    ARVB_t *arvore = populaArvB(arqArvore);
    if (!arvore)
        return 1;

    // Busca offset para prefixo tratado dado
    char *valorTratado = trataAspas(prefixo, 100);
    int64_t offSetPesquisa = pesquisaArvB(arvore, convertePrefixo(valorTratado));

    // Caso registro achado na árvore, exibe
    if (offSetPesquisa > 0)
        exibeVeiculoOffset(arqTabela, offSetPesquisa);

    // Libera memória alocada
    free(arvore); free(valorTratado);

    // Trata valor de retorno
    if (offSetPesquisa > 0)
        return 0;
    return 2;
}

/* "CREATE INDEX ... Veiculos" -> cria arquivo índice [arvore] B a partir de arquivo de [tabela] dada */
int criaArvoreVeiculos(char *tabela, char *arvore) {
    // Testa nome de arquivo e arquivo aberto
    FILE *arqTabela = fopen(tabela, "rb");
    if (!tabela || !arqTabela)
        return 1;

    // Lê cabeçalho do arquivo
    CABECALHO_VEICULOS_t *cabecalho = leCabecalhoVeiculos(arqTabela);

    // Confere status do arquivo
    if (cabecalho->status != '1') {
        fclose(arqTabela);
        destroiCabecalhoVeiculos(cabecalho);
        return 1;
    }

    // Lê contagem de registros
    if (cabecalho->nroRegs == 0) {
        fclose(arqTabela);
        destroiCabecalhoVeiculos(cabecalho);
        return -1;
    }

    // Cria e popula árvore
    ARVB_t *arvoreVeiculos = populaArvB(arvore);

    // Auxiliar de retorno
    int ret;

    // Para cada registro
    for (int i = 0; i < cabecalho->nroRegs + cabecalho->nroRems; ++i) {
        // Salva offset para adição na árvore
        int64_t offsetAtual = ftell(arqTabela);

        // Lê veículo atual da lista
        VEICULO_t *veiculoAtual = leVeiculo(arqTabela);
        if (!veiculoAtual) {
            fclose(arqTabela);
            return 1;
        }
        // Confere veículo removido
        if (!veiculoAtual->removido) {
            destroiVeiculo(veiculoAtual);
            continue;
        }
        // Adiciona veículo à árvore
        ret = adicionaRegistroArvB(arvoreVeiculos, convertePrefixo(veiculoAtual->prefixo), offsetAtual);
        destroiVeiculo(veiculoAtual);
    }

    // Libera memória alocada
    destroiCabecalhoVeiculos(cabecalho);
    fclose(arqTabela); free(arvoreVeiculos);
    return ret;
}

/* "INSERT INTO Veiculos [INDEX] ..." -> Insere informações lidas em [registro] na árvore do arquivo [arqArvore] dada presente no [offsetInsercao] */
int adicionaVeiculoArvore(char *arqArvore, char *registro, int64_t offsetInsercao) {
    // Popula árvore com arquivo dado
    ARVB_t *arvore = populaArvB(arqArvore);
    if (!arvore || !arqArvore || !registro || offsetInsercao < 0)
        return 1;

    // Gera veículo pelo registro
    VEICULO_t *tempVeiculo = regParaVeiculo(registro);
    // Confere veículo criado
    if (!tempVeiculo)
        return 1;
    // Adiciona e registra retorno
    int ret = adicionaRegistroArvB(arvore, convertePrefixo(tempVeiculo->prefixo), offsetInsercao);

    // Libera memória alocada
    free(arvore); destroiVeiculo(tempVeiculo);

    return ret;
}

/* Lê cabeçalho de veículos do [arquivo] dado */
CABECALHO_VEICULOS_t *leCabecalhoVeiculos(FILE *arquivo) {
    CABECALHO_VEICULOS_t *cabecalho = malloc(sizeof(CABECALHO_VEICULOS_t));
    if (!cabecalho)
        return NULL;

    fread(&(cabecalho->status), sizeof(char), 1, arquivo);
    /* if (cabecalho->status != '1') */
    /*     return cabecalho; */
    fread(&(cabecalho->byteOffset), sizeof(int64_t), 1, arquivo);

    // Lê contagem de registros
    fread(&(cabecalho->nroRegs), sizeof(int32_t), 1, arquivo);
    fread(&(cabecalho->nroRems), sizeof(int32_t), 1, arquivo);

    // Lê strings de cabeçalho de veículos
    cabecalho->prefixo = calloc(19, sizeof(char));
    fread(cabecalho->prefixo, sizeof(char), 18, arquivo);

    cabecalho->data = calloc(36, sizeof(char));
    fread(cabecalho->data, sizeof(char), 35, arquivo);

    cabecalho->lugares = calloc(43, sizeof(char));
    fread(cabecalho->lugares, sizeof(char), 42, arquivo);

    cabecalho->linha = calloc(27, sizeof(char));
    fread(cabecalho->linha, sizeof(char), 26, arquivo);

    cabecalho->modelo = calloc(18, sizeof(char));
    fread(cabecalho->modelo, sizeof(char), 17, arquivo);

    cabecalho->categoria = calloc(21, sizeof(char));
    fread(cabecalho->categoria, sizeof(char), 20, arquivo);

    return cabecalho;
}

/* Libera memória de [cabecalho] de veículos lido */
int destroiCabecalhoVeiculos(CABECALHO_VEICULOS_t *cabecalho) {
    if (!cabecalho)
        return 1;

    free(cabecalho->categoria);
    free(cabecalho->data);
    free(cabecalho->linha);
    free(cabecalho->lugares);
    free(cabecalho->modelo);
    free(cabecalho->prefixo);
    free(cabecalho);
    return 0;
}

RetornaChave_f retornaCodVeiculo(void *veiculo) {
    VEICULO_t *obj = (VEICULO_t *) veiculo;
    return obj->codLinha + 1;
}

/* Ordena veículos do arquivo de entrada [arqEntrada] com base no [campoOrdenacao] e escreve lista para arquivo [arqSaida] */
int ordenaVeiculos(char *arqEntrada, char *arqSaida, char *campoOrdenacao) {
    if (!arqEntrada || !arqSaida || !campoOrdenacao)
        return 1;

    FILE *tabelaVeiculos = fopen(arqEntrada, "rb");
    if (!tabelaVeiculos)
        return 1;

    CABECALHO_VEICULOS_t *cabecalho = leCabecalhoVeiculos(tabelaVeiculos);
    if (!cabecalho) {
        fclose(tabelaVeiculos);
        return 1;
    }

    if (cabecalho->status != '1') {
        fclose(tabelaVeiculos);
        destroiCabecalhoVeiculos(cabecalho);
        return 1;
    }

    int contVeiculos = 0;
    VEICULO_t **veiculos = malloc(cabecalho->nroRegs * sizeof(VEICULO_t*));

    int64_t byteOffset = 175;
    for (int i = 0; i < cabecalho->nroRegs + cabecalho->nroRems; ++i) {
        VEICULO_t *veiculo = leVeiculo(tabelaVeiculos);
        if (!veiculo)
            continue;

        veiculos[contVeiculos++] = veiculo;
        byteOffset += veiculo->tamRegistro + 5;
    }

    radixSort((void**) veiculos, sizeof(int32_t), contVeiculos, &retornaCodVeiculo);

    FILE *tabelaOrdenada = fopen(arqSaida, "wb+");

    // Inicializa cabeçalho
    int32_t nRems = 0;
    fwrite("0", sizeof(char), 1, tabelaOrdenada);
    fwrite(&byteOffset, sizeof(int64_t), 1, tabelaOrdenada);
    fwrite(&contVeiculos, sizeof(int32_t), 1, tabelaOrdenada);
    fwrite(&nRems, sizeof(int32_t), 1, tabelaOrdenada);
    fwrite(cabecalho->prefixo, sizeof(char), 18, tabelaOrdenada);
    fwrite(cabecalho->data, sizeof(char), 35, tabelaOrdenada);
    fwrite(cabecalho->lugares, sizeof(char), 42, tabelaOrdenada);
    fwrite(cabecalho->linha, sizeof(char), 26, tabelaOrdenada);
    fwrite(cabecalho->modelo, sizeof(char), 17, tabelaOrdenada);
    fwrite(cabecalho->categoria, sizeof(char), 20, tabelaOrdenada);

    for (int i = 0; i < contVeiculos; ++i) {
        fwrite(&(veiculos[i]->removido), sizeof(char), 1, tabelaOrdenada);
        fwrite(&(veiculos[i]->tamRegistro), sizeof(int32_t), 1, tabelaOrdenada);
        fwrite(veiculos[i]->prefixo, sizeof(char), 5, tabelaOrdenada);
        fwrite(veiculos[i]->dataOriginal, sizeof(char), 10, tabelaOrdenada);
        fwrite(&(veiculos[i]->quantidadeLugares), sizeof(int32_t), 1, tabelaOrdenada);
        fwrite(&(veiculos[i]->codLinha), sizeof(int32_t), 1, tabelaOrdenada);
        int32_t tamModelo = veiculos[i]->modelo ? strlen(veiculos[i]->modelo) : 0,
            tamCategoria = veiculos[i]->categoria ? strlen(veiculos[i]->categoria) : 0;
        fwrite(&tamModelo, sizeof(int32_t), 1, tabelaOrdenada);
        fwrite(veiculos[i]->modelo, sizeof(char), tamModelo, tabelaOrdenada);
        fwrite(&tamCategoria, sizeof(int32_t), 1, tabelaOrdenada);
        fwrite(veiculos[i]->categoria, sizeof(char), tamCategoria, tabelaOrdenada);
        destroiVeiculo(veiculos[i]);
    } free(veiculos);

    fseek(tabelaOrdenada, 0, SEEK_SET);
    fwrite("1", sizeof(char), 1, tabelaOrdenada);

    fclose(tabelaVeiculos); fclose(tabelaOrdenada);
    destroiCabecalhoVeiculos(cabecalho);

    return 0;
}
