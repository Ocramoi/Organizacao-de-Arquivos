#include "Linhas.h"
#include "LeLinha.h"

#include <stdio.h>
#include <string.h>

/* Escreve [arquivo] binário a partir da [tabela] dada */
int escreveTabelaLinhas(FILE *arquivo, LINHAS_t *tabela) {
    return 0;
}

/* Adiciona linha a partir de [registro] à [tabela] já criada */
LINHA_t *adicionaLinha(LINHAS_t *tabela, char *registro) {
    // Estrutura para novo linha a ser adicionado
    LINHA_t *novo = malloc(sizeof(LINHA_t));
    // Inicia tokenização do csv
    char *token;
   
    // Cria nova estrutura para a linha a ser lida e popula dados
    novo->removido = 1;
    token = strsep(&registro, ",");
    strcpy(novo->codLinha, token);
    printf("Codigo: %s\n",novo->codLinha);
    token = strsep(&registro, ",");
    strcpy(novo->aceitaCartao, token);
    printf("Aceita Cartao: %s\n",novo->aceitaCartao);
    token = strsep(&registro, ",");
    strcpy(novo->nomeLinha, token); 
    printf("Nome Linha: %s\n",novo->nomeLinha);
    token = strsep(&registro, ",");
    strcpy(novo->corLinha, token);
    printf("Nome cor: %s\n",novo->corLinha);

    //Calcula o tamanho dos registros != de calcular o tamanho da struct
    novo->tamanhoNome = strlen(novo->nomeLinha);
    novo->tamanhoCor = strlen(novo->corLinha);
    novo->tamanhoRegistro = strlen(novo->codLinha)+strlen(novo->aceitaCartao)+novo->tamanhoNome+novo->tamanhoCor ;
    printf("Tamanho total: %d Tamanho Nome: %d  Tamanho Cor: %d \n", novo->tamanhoRegistro, novo->tamanhoNome, novo->tamanhoCor);
    printf("\n");
   

    // Retorna registro adicionado
    return novo;
}

/* "CREATE TABLE" de linhas a partir do csv de nome [entrada], salvando como o arquivo binário [saida] */
int criaTabelaLinhas(char *entrada, char *saida) {
    // Abre arquivo de entrada a partir do nome
    FILE *arqEntrada = fopen(entrada, "rb");
    if (!arqEntrada) { // Confere falha na abertura dos arquivos
        // Fecha arquivos e retorna erro;
        fclose(arqEntrada);
        return 1;
    }

    // Lê cabeçalho do arquivo de entrada
    char *registro = leLinha(arqEntrada);
    // Inicia tokenização do csv
    char *variavel;    

    // Cria nova estrutura para a tabela a ser lida e popula dados
    LINHAS_t *tabela = malloc(sizeof(LINHAS_t));
    tabela->status = 0;
    tabela->byteProxReg = 0;
    tabela->nroRegistros = 0;
    tabela->nroRegRemovidos = 0;
    printf("Registro de Cabecalho: %d %ld %d %d ", tabela->status, tabela->byteProxReg, tabela->nroRegistros, tabela->nroRegRemovidos);
    variavel = strsep(&registro, ",");
    strcpy(tabela->descreveCodigo, variavel);
    printf("%s ",tabela->descreveCodigo);
    variavel = strsep(&registro, ",");
    strcpy(tabela->descreveCartao, variavel);
    printf("%s ",tabela->descreveCartao);
    variavel = strsep(&registro, ",");
    strcpy(tabela->descreveNome, variavel);
    printf("%s ",tabela->descreveNome);
    variavel = strsep(&registro, ",");
    strcpy(tabela->descreveLinha, variavel);
    printf("%s\n\n",tabela->descreveLinha);
    tabela->linhas = NULL;

    
    
    //Libera registro e posiciona o ponteiro no inicio do arquivo
    free(registro);
    fseek(arqEntrada, 0, SEEK_SET);
    registro = leLinha(arqEntrada);
    
    // Lê registros do csv linha a linha
    while (registro) {
        free(registro);
        registro = leLinha(arqEntrada);
        if (!registro) break;
        if (adicionaLinha(tabela, registro)){}
            //printf("Erro na inserção de registro!\n"); //flag
    }


    // Fecha arquivo de entrada
    fclose(arqEntrada);

    tabela->status = 1; // TODO: Conferir se o status da tabela tá sendo indicado de acordo com especifiações ujiadfsguiabguib

    // Abre arquivo de saída
    FILE *arqSaida = fopen(saida, "wb+");
    // Escreve tabela
    escreveTabelaLinhas(arqSaida, tabela);
    // Fecha arquivo
    fclose(arqSaida);

    // Retorno de sucesso
    return 0;
}
